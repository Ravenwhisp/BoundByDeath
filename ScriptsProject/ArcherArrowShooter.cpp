#include "pch.h"
#include "ArcherArrowShooter.h"
#include "ArcherAttackConfig.h"
#include "RangedEnemyController.h"
#include "ArcherArrowProjectile.h"

IMPLEMENT_SCRIPT_FIELDS(ArcherArrowShooter,
    SERIALIZED_STRING(m_arrowPrefab, "Arrow Prefab")
)

ArcherArrowShooter::ArcherArrowShooter(GameObject* owner) : Script(owner) {}

void ArcherArrowShooter::Start()
{
    m_config     = GameObjectAPI::findScript<ArcherAttackConfig>(getOwner());
    m_controller = GameObjectAPI::findScript<RangedEnemyController>(getOwner());
    m_animation  = AnimationAPI::getAnimationComponent(getOwner());
}

void ArcherArrowShooter::Update()
{
    if (!m_animation || !m_config || m_arrowPrefab.empty())
        return;

    const char* state = AnimationAPI::getActiveStateName(m_animation);
    if (!state) return;

    const bool nowInAttack = (strcmp(state, "BASIC_ATTACK") == 0);

    // State entered
    if (nowInAttack && !m_inAttack)
    {
        m_timer    = 0.0f;
        m_fired    = false;
        m_inAttack = true;
    }

    // State exited — clean up arrow if still flying
    if (!nowInAttack && m_inAttack)
    {
        if (m_arrowGO)
        {
            GameObjectAPI::removeGameObject(m_arrowGO);
            m_arrowGO = nullptr;
        }
        m_inAttack = false;
    }

    if (!m_inAttack) return;

    m_timer += Time::getDeltaTime();

    // Fire at windup time
    if (!m_fired && m_timer >= m_config->m_basicAttackWindupTime)
    {
        Transform* archerT = GameObjectAPI::getTransform(getOwner());
        Transform* targetT = m_controller ? m_controller->getCurrentTarget() : nullptr;

        if (archerT && targetT)
        {
            Vector3 archerPos = TransformAPI::getGlobalPosition(archerT);
            Vector3 targetPos = TransformAPI::getGlobalPosition(targetT);

            // Spawn from chest height, slightly toward the target
            Vector3 toTarget = targetPos - archerPos;
            toTarget.y = 0.0f;
            if (toTarget.LengthSquared() > 0.0001f) toTarget.Normalize();

            Vector3 spawnPos  = archerPos;
            spawnPos.x       += toTarget.x * 0.5f;
            spawnPos.y       += 1.2f;
            spawnPos.z       += toTarget.z * 0.5f;

            m_arrowGO = GameObjectAPI::instantiatePrefab(m_arrowPrefab.c_str(), spawnPos, Vector3::Zero);
            if (m_arrowGO)
            {
                ArcherArrowProjectile* arrow = GameObjectAPI::findScript<ArcherArrowProjectile>(m_arrowGO);
                if (arrow)
                {
                    Vector3 dest = targetPos;
                    dest.y += 1.0f;  // aim at player chest, not feet
                    arrow->launch(spawnPos, dest, 10.0f);
                }
            }
        }
        m_fired = true;
    }

    // Remove arrow once it has arrived
    if (m_arrowGO)
    {
        ArcherArrowProjectile* arrow = GameObjectAPI::findScript<ArcherArrowProjectile>(m_arrowGO);
        if (arrow && arrow->hasArrived())
        {
            GameObjectAPI::removeGameObject(m_arrowGO);
            m_arrowGO = nullptr;
        }
    }
}

IMPLEMENT_SCRIPT(ArcherArrowShooter)

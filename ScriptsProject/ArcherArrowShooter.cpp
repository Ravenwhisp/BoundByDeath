#include "pch.h"
#include "ArcherArrowShooter.h"
#include "ArcherAttackConfig.h"
#include "RangedEnemyController.h"
#include "ArcherArrowProjectile.h"

IMPLEMENT_SCRIPT_FIELDS(ArcherArrowShooter,
    SERIALIZED_STRING(m_arrowPrefab, "Arrow Prefab"),
    SERIALIZED_STRING(m_trailPrefab, "Trail Particle Prefab")
)

ArcherArrowShooter::ArcherArrowShooter(GameObject* owner) : Script(owner) {}

void ArcherArrowShooter::Start()
{
    m_config     = GameObjectAPI::findScript<ArcherAttackConfig>(getOwner());
    m_controller = GameObjectAPI::findScript<RangedEnemyController>(getOwner());
    m_animation  = AnimationAPI::getAnimationComponent(getOwner());
}

static void removeIfValid(GameObject*& go)
{
    if (go)
    {
        GameObjectAPI::removeGameObject(go);
        go = nullptr;
    }
}

void ArcherArrowShooter::Update()
{
    if (!m_animation || !m_config || m_arrowPrefab.empty()) return;

    const char* state = AnimationAPI::getActiveStateName(m_animation);
    if (!state) return;

    const bool nowInAttack = (strcmp(state, "BASIC_ATTACK") == 0);

    if (nowInAttack && !m_inAttack)
    {
        m_timer    = 0.0f;
        m_fired    = false;
        m_inAttack = true;
    }

    if (!nowInAttack && m_inAttack)
    {
        removeIfValid(m_arrowGO);
        removeIfValid(m_trailGO);
        m_inAttack = false;
    }

    if (!m_inAttack) return;

    m_timer += Time::getDeltaTime();

    if (!m_fired && m_timer >= m_config->m_basicAttackWindupTime)
    {
        Transform* archerT = GameObjectAPI::getTransform(getOwner());
        Transform* targetT = m_controller ? m_controller->getCurrentTarget() : nullptr;

        if (archerT && targetT)
        {
            Vector3 archerPos = TransformAPI::getGlobalPosition(archerT);
            Vector3 targetPos = TransformAPI::getGlobalPosition(targetT);

            Vector3 toTarget = targetPos - archerPos;
            toTarget.y = 0.0f;
            if (toTarget.LengthSquared() > 0.0001f) toTarget.Normalize();

            Vector3 spawnPos  = archerPos;
            spawnPos.x       += toTarget.x * 0.5f;
            spawnPos.y       += 1.2f;
            spawnPos.z       += toTarget.z * 0.5f;

            Vector3 dest  = targetPos;
            dest.y       += 1.0f;

            m_arrowGO = GameObjectAPI::instantiatePrefab(m_arrowPrefab.c_str(), spawnPos, Vector3::Zero);
            if (m_arrowGO)
            {
                ArcherArrowProjectile* arrow = GameObjectAPI::findScript<ArcherArrowProjectile>(m_arrowGO);
                if (arrow) arrow->launch(spawnPos, dest, 10.0f);
            }

            if (!m_trailPrefab.empty())
                m_trailGO = GameObjectAPI::instantiatePrefab(m_trailPrefab.c_str(), spawnPos, Vector3::Zero);
        }
        m_fired = true;
    }

    if (m_arrowGO && m_trailGO)
    {
        Transform* arrowT = GameObjectAPI::getTransform(m_arrowGO);
        Transform* trailT = GameObjectAPI::getTransform(m_trailGO);
        if (arrowT && trailT)
        {
            TransformAPI::setGlobalPosition(trailT, TransformAPI::getGlobalPosition(arrowT));
            TransformAPI::setGlobalRotationEuler(trailT, TransformAPI::getGlobalEulerDegrees(arrowT));
        }
    }

    if (m_arrowGO)
    {
        ArcherArrowProjectile* arrow = GameObjectAPI::findScript<ArcherArrowProjectile>(m_arrowGO);
        if (arrow && arrow->hasArrived())
        {
            removeIfValid(m_arrowGO);
            removeIfValid(m_trailGO);
        }
    }
}

IMPLEMENT_SCRIPT(ArcherArrowShooter)

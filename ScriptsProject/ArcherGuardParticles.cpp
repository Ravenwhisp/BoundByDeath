#include "pch.h"
#include "ArcherGuardParticles.h"
#include "ArcherAttackConfig.h"
#include "ArcherArrowProjectile.h"
#include "RangedEnemyController.h"

IMPLEMENT_SCRIPT_FIELDS(ArcherGuardParticles,
    SERIALIZED_STRING(m_volleyPrefab, "Volley Particle Prefab"),
    SERIALIZED_STRING(m_chargePrefab, "Charge Particle Prefab"),
    SERIALIZED_STRING(m_arrowPrefab,  "Barrage Arrow Prefab")
)

ArcherGuardParticles::ArcherGuardParticles(GameObject* owner) : Script(owner) {}

void ArcherGuardParticles::Start()
{
    m_config     = GameObjectAPI::findScript<ArcherAttackConfig>(getOwner());
    m_controller = GameObjectAPI::findScript<RangedEnemyController>(getOwner());
    m_animation  = AnimationAPI::getAnimationComponent(getOwner());

    if (!m_config)     Debug::error("[ArcherGuardParticles] ArcherAttackConfig not found.");
    if (!m_controller) Debug::error("[ArcherGuardParticles] RangedEnemyController not found.");
    if (!m_animation)  Debug::error("[ArcherGuardParticles] AnimationComponent not found.");
}

void ArcherGuardParticles::Update()
{
    if (!m_animation || !m_config) return;
    const char* state = AnimationAPI::getActiveStateName(m_animation);
    if (!state) return;

    updateBarrage(state);
    updateCharge(state);
}

static const Vector3 k_barrageOffsets[] = {
    Vector3( 0.0f, 0.0f,  0.0f),
    Vector3( 1.2f, 0.0f,  0.0f),
    Vector3(-1.2f, 0.0f,  0.0f),
    Vector3( 0.0f, 0.0f,  1.2f),
    Vector3( 0.0f, 0.0f, -1.2f),
};
static const int k_barrageArrowCount = 5;
static const float k_barrageSpawnHeight = 8.0f; 

void ArcherGuardParticles::updateBarrage(const char* state)
{
    const bool nowInBarrage = (strcmp(state, "ARROWBARRAGE") == 0);

    if (nowInBarrage && !m_inBarrage)
    {
        m_barrageTimer       = 0.0f;
        m_barrageImpactFired = false;
        m_barrageArrowsFired = false;
        m_barrageImpactPos   = Vector3(0.0f, 0.0f, 0.0f);
        m_barrageArrows.clear();
        m_inBarrage          = true;
    }

    if (!nowInBarrage && m_inBarrage)
    {
        for (GameObject* go : m_barrageArrows)
            if (go) GameObjectAPI::removeGameObject(go);
        m_barrageArrows.clear();
        m_inBarrage = false;
    }

    if (!m_inBarrage) return;

    m_barrageTimer += Time::getDeltaTime();

    if (!m_barrageArrowsFired && m_barrageTimer >= m_config->m_arrowBarrageThrowTime
        && m_barrageImpactPos.LengthSquared() < 0.001f)
    {
        Transform* targetT = m_controller->getCurrentTarget();
        if (targetT)
            m_barrageImpactPos = TransformAPI::getGlobalPosition(targetT);
        else
        {
            Transform* t = GameObjectAPI::getTransform(getOwner());
            if (t) m_barrageImpactPos = TransformAPI::getGlobalPosition(t);
        }
    }

    if (!m_barrageArrowsFired && !m_arrowPrefab.empty()
        && m_barrageImpactPos.LengthSquared() > 0.001f
        && m_barrageTimer >= m_config->m_arrowBarrageThrowTime)
    {
        const float landDelay = m_config->m_arrowBarrageLandDelay;
        const float arrowSpeed = k_barrageSpawnHeight / (landDelay > 0.0f ? landDelay : 1.0f);

        for (int i = 0; i < k_barrageArrowCount; ++i)
        {
            Vector3 impactTarget = m_barrageImpactPos + k_barrageOffsets[i];
            Vector3 spawnPos     = impactTarget;
            spawnPos.y          += k_barrageSpawnHeight;

            GameObject* arrowGO = GameObjectAPI::instantiatePrefab(
                m_arrowPrefab.c_str(), spawnPos, Vector3::Zero);

            if (arrowGO)
            {
                ArcherArrowProjectile* arrow = GameObjectAPI::findScript<ArcherArrowProjectile>(arrowGO);
                if (arrow) arrow->launch(spawnPos, impactTarget, arrowSpeed);
                m_barrageArrows.push_back(arrowGO);
            }
        }
        m_barrageArrowsFired = true;
    }

    const float impactTime = m_config->m_arrowBarrageThrowTime + m_config->m_arrowBarrageLandDelay;
    if (!m_barrageImpactFired && m_barrageTimer >= impactTime)
    {
        if (!m_volleyPrefab.empty())
            GameObjectAPI::instantiatePrefab(m_volleyPrefab.c_str(), m_barrageImpactPos, Vector3::Zero);

        for (GameObject*& go : m_barrageArrows)
        {
            if (go) { GameObjectAPI::removeGameObject(go); go = nullptr; }
        }
        m_barrageArrows.clear();
        m_barrageImpactFired = true;
    }
}

void ArcherGuardParticles::updateCharge(const char* state)
{
    if (m_chargePrefab.empty()) return;

    const bool nowInCharge = (strcmp(state, "SOMERSAULT") == 0);

    if (nowInCharge && !m_inCharge)
    {
        Transform* t = GameObjectAPI::getTransform(getOwner());
        Vector3 pos = t ? TransformAPI::getGlobalPosition(t) : Vector3::Zero;
        m_chargeParticleGO = GameObjectAPI::instantiatePrefab(m_chargePrefab.c_str(), pos, Vector3::Zero);
        m_inCharge = true;
    }

    if (m_inCharge && m_chargeParticleGO)
    {
        Transform* archerT   = GameObjectAPI::getTransform(getOwner());
        Transform* particleT = GameObjectAPI::getTransform(m_chargeParticleGO);
        if (archerT && particleT)
            TransformAPI::setGlobalPosition(particleT, TransformAPI::getGlobalPosition(archerT));
    }

    if (!nowInCharge && m_inCharge)
    {
        if (m_chargeParticleGO)
        {
            GameObjectAPI::removeGameObject(m_chargeParticleGO);
            m_chargeParticleGO = nullptr;
        }
        m_inCharge = false;
    }
}

IMPLEMENT_SCRIPT(ArcherGuardParticles)

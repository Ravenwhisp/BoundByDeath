#include "pch.h"
#include "ArcherGuardParticles.h"
#include "ArcherArrowProjectile.h"

IMPLEMENT_SCRIPT_FIELDS(ArcherGuardParticles,
    SERIALIZED_ASSET_REF(m_trailPrefab,  "Trail Particle Prefab", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_barrageImpactPrefab, "Barrage Impact Prefab", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_somersaultPrefab, "Somesault Particle Prefab", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_arrowBarragePrefab,  "Barrage Arrow Prefab", AssetType::PREFAB),
    SERIALIZED_FLOAT(m_impactParticleLifetime, "Impact Particle Lifetime", 0.0f, 10.0f, 0.1f)
)

ArcherGuardParticles::ArcherGuardParticles(GameObject* owner) : Script(owner) {}

void ArcherGuardParticles::Start() {}

void ArcherGuardParticles::Update()
{
    if (!m_impactParticleGO)
    {
        return;
    }

    m_impactParticleTimer -= Time::getDeltaTime();

    if (m_impactParticleTimer <= 0.0f)
    {
        GameObjectAPI::removeGameObject(m_impactParticleGO);
        m_impactParticleGO = nullptr;
        m_impactParticleTimer = 0.0f;
    }
}

// ── Basic attack trail ────────────────────────────────────────────────────────

void ArcherGuardParticles::spawnBasicAttackTrail(const Vector3& pos)
{
    stopBasicAttackTrail();

    if (!m_trailPrefab.m_id.isValid())
    {
        return;
    }

    m_trailGO = GameObjectAPI::instantiatePrefab(m_trailPrefab.m_id, pos, Vector3::Zero);
}

void ArcherGuardParticles::syncBasicAttackTrail(const Vector3& pos, const Vector3& eulerDeg)
{
    if (!m_trailGO) return;
    Transform* t = GameObjectAPI::getTransform(m_trailGO);
    if (t)
    {
        TransformAPI::setGlobalPosition(t, pos);
        TransformAPI::setGlobalRotationEuler(t, eulerDeg);
    }
}

void ArcherGuardParticles::stopBasicAttackTrail()
{
    if (m_trailGO) { GameObjectAPI::removeGameObject(m_trailGO); m_trailGO = nullptr; } 
}

// ── Barrage ───────────────────────────────────────────────────────────────────

static const Vector3 k_barrageOffsets[] = {
    Vector3( 0.0f, 0.0f,  0.0f),
    Vector3( 1.2f, 0.0f,  0.0f),
    Vector3(-1.2f, 0.0f,  0.0f),
    Vector3( 0.0f, 0.0f,  1.2f),
    Vector3( 0.0f, 0.0f, -1.2f),
};
static const int   k_barrageArrowCount  = 5;
static const float k_barrageSpawnHeight = 8.0f;

void ArcherGuardParticles::spawnBarrageArrows(const Vector3& impactPos, float landDelay)
{
    stopBarrageArrows();
    if (!m_arrowBarragePrefab.m_id.isValid()) return;

    const float speed = k_barrageSpawnHeight / (landDelay > 0.0f ? landDelay : 1.0f);

    for (int i = 0; i < k_barrageArrowCount; ++i)
    {
        Vector3 target   = impactPos + k_barrageOffsets[i];
        Vector3 spawnPos = target;
        spawnPos.y      += k_barrageSpawnHeight;

        GameObject* go = GameObjectAPI::instantiatePrefab(m_arrowBarragePrefab.m_id, spawnPos, Vector3::Zero);
        if (go)
        {
            ArcherArrowProjectile* arrow = GameObjectAPI::findScript<ArcherArrowProjectile>(go);
            if (arrow) arrow->launch(spawnPos, target, speed);
            m_barrageArrows.push_back(go);
        }
    }
}

void ArcherGuardParticles::spawnImpactParticle(const Vector3& impactPos)
{
    stopBarrageArrows();

    if (!m_barrageImpactPrefab.m_id.isValid())
    {
        return;
    }

    if (m_impactParticleGO)
    {
        GameObjectAPI::removeGameObject(m_impactParticleGO);
        m_impactParticleGO = nullptr;
    }

    m_impactParticleGO = GameObjectAPI::instantiatePrefab(m_barrageImpactPrefab.m_id, impactPos, Vector3::Zero);

    if (m_impactParticleGO)
    {
        m_impactParticleTimer = m_impactParticleLifetime;
    }
}

void ArcherGuardParticles::stopBarrageArrows()
{
    for (GameObject* go : m_barrageArrows)
        if (go) GameObjectAPI::removeGameObject(go);
    m_barrageArrows.clear();
}

// ── Charge / somersault ───────────────────────────────────────────────────────

void ArcherGuardParticles::startChargeParticle()
{
    stopChargeParticle();

    if (!m_somersaultPrefab.m_id.isValid())
    {
        Debug::warn("[ArcherGuardParticles] Somersault particle prefab is missing.");
        return;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());

    if (!ownerTransform)
    {
        return;
    }

    const Vector3 position = TransformAPI::getGlobalPosition(ownerTransform);
    const Vector3 rotation = TransformAPI::getGlobalEulerDegrees(ownerTransform);

    m_chargeParticleGO = GameObjectAPI::instantiatePrefab(m_somersaultPrefab.m_id, position, rotation);

    m_chargeParticleTransform = GameObjectAPI::getTransform(m_chargeParticleGO);
}

void ArcherGuardParticles::updateChargeParticle()
{
    if (!m_chargeParticleTransform)
    {
        return;
    }

    Transform* archerTransform = GameObjectAPI::getTransform(getOwner());

    if (!archerTransform)
    {
        return;
    }

    TransformAPI::setGlobalPosition(m_chargeParticleTransform, TransformAPI::getGlobalPosition(archerTransform));
    TransformAPI::setGlobalRotationEuler(m_chargeParticleTransform, TransformAPI::getGlobalEulerDegrees(archerTransform));
}

void ArcherGuardParticles::stopChargeParticle()
{
    if (m_chargeParticleGO)
    {
        GameObjectAPI::removeGameObject(m_chargeParticleGO);
    }

    m_chargeParticleGO = nullptr;
    m_chargeParticleTransform = nullptr;
}

IMPLEMENT_SCRIPT(ArcherGuardParticles)

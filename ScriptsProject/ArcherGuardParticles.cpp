#include "pch.h"
#include "ArcherGuardParticles.h"

IMPLEMENT_SCRIPT_FIELDS(ArcherGuardParticles,
    SERIALIZED_ASSET_REF(m_trailPrefab, "Trail Particle Prefab", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_barrageFloorPrefab, "Barrage Floor Particle Prefab", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_barrageImpactPrefab, "Barrage Impact Particle Prefab", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_somersaultPrefab, "Somersault Particle Prefab", AssetType::PREFAB),
    SERIALIZED_FLOAT(m_barrageFloorYOffset, "Barrage Floor Y Offset", -5.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_barrageImpactYOffset, "Barrage Impact Y Offset", -5.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_barrageImpactLifetime, "Barrage Impact Lifetime", 0.0f, 10.0f, 0.1f)
)

ArcherGuardParticles::ArcherGuardParticles(GameObject* owner)
    : Script(owner)
{
}

void ArcherGuardParticles::Start()
{
}

void ArcherGuardParticles::Update()
{
    if (!m_barrageImpactParticle)
    {
        return;
    }

    m_barrageImpactTimer -= Time::getDeltaTime();

    if (m_barrageImpactTimer <= 0.0f)
    {
        GameObjectAPI::removeGameObject(m_barrageImpactParticle);
        m_barrageImpactParticle = nullptr;
        m_barrageImpactTimer = 0.0f;
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
    if (!m_trailGO)
    {
        return;
    }

    Transform* trailTransform = GameObjectAPI::getTransform(m_trailGO);

    if (!trailTransform)
    {
        return;
    }

    TransformAPI::setGlobalPosition(trailTransform, pos);
    TransformAPI::setGlobalRotationEuler(trailTransform, eulerDeg);
}

void ArcherGuardParticles::stopBasicAttackTrail()
{
    if (m_trailGO)
    {
        GameObjectAPI::removeGameObject(m_trailGO);
    }

    m_trailGO = nullptr;
}

// ── Arrow barrage ─────────────────────────────────────────────────────────────

void ArcherGuardParticles::startBarrageFloorParticle(const Vector3& position)
{
    stopBarrageFloorParticle();

    if (!m_barrageFloorPrefab.m_id.isValid())
    {
        Debug::warn("[ArcherGuardParticles] Barrage floor particle prefab is missing.");
        return;
    }

    Vector3 particlePosition = position;
    particlePosition.y += m_barrageFloorYOffset;

    m_barrageFloorParticle = GameObjectAPI::instantiatePrefab(m_barrageFloorPrefab.m_id, particlePosition, Vector3::Zero);
}

void ArcherGuardParticles::stopBarrageFloorParticle()
{
    if (m_barrageFloorParticle)
    {
        GameObjectAPI::removeGameObject(m_barrageFloorParticle);
    }

    m_barrageFloorParticle = nullptr;
}

void ArcherGuardParticles::playBarrageImpactParticle(const Vector3& position)
{
    if (!m_barrageImpactPrefab.m_id.isValid())
    {
        Debug::warn("[ArcherGuardParticles] Barrage impact particle prefab is missing.");
        return;
    }

    if (m_barrageImpactParticle)
    {
        GameObjectAPI::removeGameObject(m_barrageImpactParticle);
        m_barrageImpactParticle = nullptr;
    }

    Vector3 particlePosition = position;
    particlePosition.y += m_barrageImpactYOffset;

    m_barrageImpactParticle = GameObjectAPI::instantiatePrefab(m_barrageImpactPrefab.m_id, particlePosition, Vector3::Zero);

    if (!m_barrageImpactParticle)
    {
        m_barrageImpactTimer = 0.0f;
        return;
    }

    m_barrageImpactTimer = m_barrageImpactLifetime;
}

// ── Somersault ────────────────────────────────────────────────────────────────

void ArcherGuardParticles::startChargeParticle()
{
    stopChargeParticle();

    if (!m_somersaultPrefab.m_id.isValid())
    {
        Debug::warn("[ArcherGuardParticles] Somersault particle prefab is missing.");
        return;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());

    const Vector3 position = TransformAPI::getGlobalPosition(ownerTransform);
    const Vector3 rotation = TransformAPI::getGlobalEulerDegrees(ownerTransform);

    m_somersaultParticle = GameObjectAPI::instantiatePrefab(m_somersaultPrefab.m_id, position, rotation);

    if (!m_somersaultParticle)
    {
        return;
    }

    m_somersaultParticleTransform = GameObjectAPI::getTransform(m_somersaultParticle);
}

void ArcherGuardParticles::updateChargeParticle()
{
    if (!m_somersaultParticleTransform)
    {
        return;
    }

    Transform* archerTransform = GameObjectAPI::getTransform(getOwner());

    if (!archerTransform)
    {
        return;
    }

    TransformAPI::setGlobalPosition(m_somersaultParticleTransform, TransformAPI::getGlobalPosition(archerTransform));
    TransformAPI::setGlobalRotationEuler(m_somersaultParticleTransform, TransformAPI::getGlobalEulerDegrees(archerTransform));
}

void ArcherGuardParticles::stopChargeParticle()
{
    if (m_somersaultParticle)
    {
        GameObjectAPI::removeGameObject(m_somersaultParticle);
    }

    m_somersaultParticle = nullptr;
    m_somersaultParticleTransform = nullptr;
}

IMPLEMENT_SCRIPT(ArcherGuardParticles)

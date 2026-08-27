#include "pch.h"
#include "SkeletonParticles.h"
#include "ParticleLifecycle.h"

IMPLEMENT_SCRIPT_FIELDS(SkeletonParticles,
    SERIALIZED_ASSET_REF(m_reviveParticlePrefab, "Revive Particle Prefab", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_shockwaveParticlePrefab, "Shockwave Particle Prefab", AssetType::PREFAB),
    SERIALIZED_FLOAT(m_reviveYOffset, "Revive Y Offset", -5.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_reviveForwardOffset, "Revive Forward Offset", -5.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_shockwaveYOffset, "Shockwave Y Offset", -5.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_shockwaveForwardOffset, "Shockwave Forward Offset", -5.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_shockwaveLifetime, "Shockwave Lifetime", 0.0f, 10.0f, 0.1f)
)

SkeletonParticles::SkeletonParticles(GameObject* owner)
    : Script(owner)
{
}

void SkeletonParticles::Start()
{
    m_ownerTransform = GameObjectAPI::getTransform(getOwner());

    if (!m_ownerTransform)
    {
        Debug::warn("[SkeletonParticles] Owner transform not found on '%s'.", GameObjectAPI::getName(getOwner()));
    }
}

void SkeletonParticles::OnGameStop()
{
    ParticleLifecycle::destroy(m_reviveParticle);
    m_reviveParticleTransform = nullptr;
    ParticleLifecycle::destroy(m_shockwaveParticle);
}

void SkeletonParticles::Update()
{
    if (m_reviveParticle && GameObjectAPI::isActiveSelf(m_reviveParticle))
    {
        updateReviveParticle();
    }

    if (!m_shockwaveParticle)
    {
        return;
    }

    m_shockwaveTimer -= Time::getDeltaTime();

    if (m_shockwaveTimer <= 0.0f)
    {
        removeShockwaveParticle();
    }
}

void SkeletonParticles::ensureReviveParticle()
{
    if (!m_ownerTransform)
    {
        m_ownerTransform = GameObjectAPI::getTransform(getOwner());
    }

    ParticleLifecycle::ensurePersistent(
        m_reviveParticle,
        m_reviveParticlePrefab.m_id,
        getReviveParticlePosition(),
        getOwnerRotation(),
        nullptr
    );

    if (m_reviveParticle)
    {
        m_reviveParticleTransform = GameObjectAPI::getTransform(m_reviveParticle);
    }
}

void SkeletonParticles::startReviveParticle()
{
    if (!m_reviveParticlePrefab.m_id.isValid())
    {
        Debug::warn("[SkeletonParticles] Revive particle prefab is missing on '%s'.", GameObjectAPI::getName(getOwner()));
        return;
    }

    ensureReviveParticle();

    if (!m_reviveParticle)
    {
        Debug::warn("[SkeletonParticles] Could not instantiate revive particle on '%s'.", GameObjectAPI::getName(getOwner()));
        return;
    }

    updateReviveParticle();
    ParticleLifecycle::activate(m_reviveParticle);
}

void SkeletonParticles::stopReviveParticle()
{
    ParticleLifecycle::deactivate(m_reviveParticle);
}

void SkeletonParticles::playShockwaveParticle()
{
    if (!m_shockwaveParticlePrefab.m_id.isValid())
    {
        Debug::warn("[SkeletonParticles] Shockwave particle prefab is missing on '%s'.", GameObjectAPI::getName(getOwner()));
        return;
    }

    if (!m_ownerTransform)
    {
        m_ownerTransform = GameObjectAPI::getTransform(getOwner());

        if (!m_ownerTransform)
        {
            return;
        }
    }

    removeShockwaveParticle();

    m_shockwaveParticle = GameObjectAPI::instantiatePrefab(
        m_shockwaveParticlePrefab.m_id,
        getShockwaveParticlePosition(),
        getOwnerRotation());

    if (!m_shockwaveParticle)
    {
        Debug::warn("[SkeletonParticles] Could not instantiate shockwave particle on '%s'.", GameObjectAPI::getName(getOwner()));
        return;
    }

    m_shockwaveTimer = m_shockwaveLifetime;
}

void SkeletonParticles::updateReviveParticle()
{
    if (!m_reviveParticleTransform)
    {
        return;
    }

    TransformAPI::setGlobalPosition(m_reviveParticleTransform, getReviveParticlePosition());
    TransformAPI::setGlobalRotationEuler(m_reviveParticleTransform, getOwnerRotation());
}

void SkeletonParticles::removeShockwaveParticle()
{
    if (m_shockwaveParticle)
    {
        GameObjectAPI::removeGameObject(m_shockwaveParticle);
    }

    m_shockwaveParticle = nullptr;
    m_shockwaveTimer = 0.0f;
}

Vector3 SkeletonParticles::getReviveParticlePosition() const
{
    if (!m_ownerTransform)
    {
        return Vector3::Zero;
    }

    const Vector3 ownerPosition = TransformAPI::getGlobalPosition(m_ownerTransform);
    const Vector3 ownerForward = TransformAPI::getForward(m_ownerTransform);

    return Vector3(ownerPosition.x + ownerForward.x * m_reviveForwardOffset, ownerPosition.y + m_reviveYOffset, ownerPosition.z + ownerForward.z * m_reviveForwardOffset);
}

Vector3 SkeletonParticles::getShockwaveParticlePosition() const
{
    if (!m_ownerTransform)
    {
        return Vector3::Zero;
    }

    const Vector3 ownerPosition = TransformAPI::getGlobalPosition(m_ownerTransform);
    const Vector3 ownerForward = TransformAPI::getForward(m_ownerTransform);

    return Vector3(ownerPosition.x + ownerForward.x * m_shockwaveForwardOffset, ownerPosition.y + m_shockwaveYOffset, ownerPosition.z + ownerForward.z * m_shockwaveForwardOffset);
}

Vector3 SkeletonParticles::getOwnerRotation() const
{
    if (!m_ownerTransform)
    {
        return Vector3::Zero;
    }

    return TransformAPI::getGlobalEulerDegrees(m_ownerTransform);
}

IMPLEMENT_SCRIPT(SkeletonParticles)
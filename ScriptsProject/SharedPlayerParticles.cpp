#include "pch.h"
#include "SharedPlayerParticles.h"

IMPLEMENT_SCRIPT_FIELDS(SharedPlayerParticles,
    SERIALIZED_ASSET_REF(m_damageParticlePrefab, "Damage Particle Prefab", AssetType::PREFAB),
    SERIALIZED_FLOAT(m_damageParticleLifetime, "Damage Particle Lifetime", 0.0f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_damageParticleCooldown, "Damage Particle Cooldown", 0.0f, 5.0f, 0.05f)
)

SharedPlayerParticles::SharedPlayerParticles(GameObject* owner)
    : Script(owner)
{
}

void SharedPlayerParticles::Update()
{
    const float deltaTime = Time::getDeltaTime();

    if (m_damageParticleCooldownTimer > 0.0f)
    {
        m_damageParticleCooldownTimer -= deltaTime;
    }

    if (m_activeDamageParticle == nullptr)
    {
        return;
    }

    if (m_ownerTransform != nullptr && m_activeDamageParticleTransform != nullptr)
    {
        const Vector3 ownerPosition = TransformAPI::getGlobalPosition(m_ownerTransform);
        TransformAPI::setGlobalPosition(m_activeDamageParticleTransform, ownerPosition);
    }

    m_damageParticleTimer -= deltaTime;

    if (m_damageParticleTimer <= 0.0f)
    {
        GameObjectAPI::removeGameObject(m_activeDamageParticle);

        m_activeDamageParticle = nullptr;
        m_activeDamageParticleTransform = nullptr;
        m_damageParticleTimer = 0.0f;
    }
}

void SharedPlayerParticles::playDamageParticle()
{
    if (m_damageParticleCooldownTimer > 0.0f)
    {
        return;
    }

    if (m_ownerTransform == nullptr)
    {
        m_ownerTransform = GameObjectAPI::getTransform(getOwner());

        if (m_ownerTransform == nullptr)
        {
            Debug::warn("PlayerParticles on '%s' could not find the owner transform.", GameObjectAPI::getName(getOwner()));
            return;
        }
    }

    if (m_activeDamageParticle != nullptr)
    {
        GameObjectAPI::removeGameObject(m_activeDamageParticle);

        m_activeDamageParticle = nullptr;
        m_activeDamageParticleTransform = nullptr;
    }

    const Vector3 spawnPosition = TransformAPI::getGlobalPosition(m_ownerTransform);
    const Vector3 spawnRotation(0.0f, 0.0f, 0.0f);

    m_activeDamageParticle = GameObjectAPI::instantiatePrefab(m_damageParticlePrefab.m_id, spawnPosition, spawnRotation);

    if (m_activeDamageParticle == nullptr)
    {
        Debug::warn("PlayerParticles on '%s' could not instantiate the damage particle.", GameObjectAPI::getName(getOwner()));
        return;
    }

    m_activeDamageParticleTransform = GameObjectAPI::getTransform(m_activeDamageParticle);
    m_damageParticleTimer = m_damageParticleLifetime;
    m_damageParticleCooldownTimer = m_damageParticleCooldown;
}

IMPLEMENT_SCRIPT(SharedPlayerParticles)
#include "pch.h"
#include "SummonerParticles.h"

IMPLEMENT_SCRIPT_FIELDS(SummonerParticles,
    SERIALIZED_ASSET_REF(m_summonParticlePrefab, "Summon Particle Prefab", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_teleportParticlePrefab, "Teleport Particle Prefab", AssetType::PREFAB),
    SERIALIZED_FLOAT(m_summonParticleLifetime, "Summon Particle Lifetime", 0.0f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_teleportParticleLifetime, "Teleport Particle Lifetime", 0.0f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_summonYOffset, "Summon Particle Y Offset", -5.0f, 5.0f, 0.05f)
)

SummonerParticles::SummonerParticles(GameObject* owner)
    : Script(owner)
{
}

void SummonerParticles::Update()
{
    updateTimedParticles(Time::getDeltaTime());
}

void SummonerParticles::playSummonParticle(const Vector3& position)
{
    if (!m_summonParticlePrefab.m_id.isValid())
    {
        Debug::warn("[SummonerParticles] Summon particle prefab is missing on '%s'.", GameObjectAPI::getName(getOwner()));
        return;
    }

    Vector3 spawnPosition = position;
    spawnPosition.y += m_summonYOffset;

    spawnTimedParticle(m_summonParticlePrefab, spawnPosition, Vector3::Zero, m_summonParticleLifetime);
}

void SummonerParticles::playTeleportParticle(const Vector3& position)
{
    if (!m_teleportParticlePrefab.m_id.isValid())
    {
        Debug::warn("[SummonerParticles] Teleport particle prefab is missing on '%s'.", GameObjectAPI::getName(getOwner()));
        return;
    }

    spawnTimedParticle(m_teleportParticlePrefab, position, getOwnerRotation(), m_teleportParticleLifetime);
}

void SummonerParticles::spawnTimedParticle(const PrefabRef& prefab, const Vector3& position, const Vector3& rotation, float lifetime)
{
    GameObject* particle = GameObjectAPI::instantiatePrefab(prefab.m_id, position, rotation);

    if (!particle)
    {
        Debug::warn("[SummonerParticles] Could not instantiate particle on '%s'.", GameObjectAPI::getName(getOwner()));
        return;
    }

    TimedParticle timedParticle;
    timedParticle.gameObject = particle;
    timedParticle.timer = lifetime;

    m_timedParticles.push_back(timedParticle);
}

void SummonerParticles::updateTimedParticles(float deltaTime)
{
    for (size_t i = 0; i < m_timedParticles.size();)
    {
        TimedParticle& particle = m_timedParticles[i];
        particle.timer -= deltaTime;

        if (particle.timer <= 0.0f)
        {
            if (particle.gameObject)
            {
                GameObjectAPI::removeGameObject(particle.gameObject);
            }

            m_timedParticles.erase(m_timedParticles.begin() + i);
            continue;
        }

        ++i;
    }
}

Vector3 SummonerParticles::getOwnerRotation() const
{
    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());

    if (!ownerTransform)
    {
        return Vector3::Zero;
    }

    return TransformAPI::getGlobalEulerDegrees(ownerTransform);
}

IMPLEMENT_SCRIPT(SummonerParticles) 
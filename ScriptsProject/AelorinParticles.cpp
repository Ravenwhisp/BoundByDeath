#include "pch.h"
#include "AelorinParticles.h"

#include <cmath>

IMPLEMENT_SCRIPT_FIELDS(AelorinParticles,
    FIELD_GROUP_COLLAPSE("Phase Transformation Prefabs",
        SERIALIZED_ASSET_REF(m_soulPillarPrefab, "Soul Pillar Prefab", AssetType::PREFAB),
        SERIALIZED_ASSET_REF(m_soulsAuraPrefab, "Souls Aura Prefab", AssetType::PREFAB),
        SERIALIZED_ASSET_REF(m_turningEmbers1Prefab, "Turning Embers 1 Prefab", AssetType::PREFAB),
        SERIALIZED_ASSET_REF(m_turningEmbers2Prefab, "Turning Embers 2 Prefab", AssetType::PREFAB),
        SERIALIZED_ASSET_REF(m_turningShockwave1Prefab, "Turning Shockwave 1 Prefab", AssetType::PREFAB),
        SERIALIZED_ASSET_REF(m_turningShockwave2Prefab, "Turning Shockwave 2 Prefab", AssetType::PREFAB),
        SERIALIZED_ASSET_REF(m_phase2AuraPrefab, "Phase 2 Aura Prefab", AssetType::PREFAB)
    ),

    FIELD_GROUP_COLLAPSE("Phase Transformation Timing",
        SERIALIZED_INT(m_soulPillarCount, "Soul Pillar Count"),
        SERIALIZED_FLOAT(m_soulPillarRadius, "Soul Pillar Radius", 0.0f, 20.0f, 0.05f),
        SERIALIZED_VEC3(m_soulPillarScale, "Soul Pillar Scale"),
        SERIALIZED_VEC3(m_soulPillarOffset, "Soul Pillar Offset"),
        SERIALIZED_FLOAT(m_shockwave1Delay, "Shockwave 1 Delay", 0.0f, 10.0f, 0.05f),
        SERIALIZED_FLOAT(m_shockwave2Delay, "Shockwave 2 Delay", 0.0f, 10.0f, 0.05f),
        SERIALIZED_FLOAT(m_soulPillarDuration, "Soul Pillar Duration", 0.0f, 20.0f, 0.05f),
        SERIALIZED_FLOAT(m_embersDuration, "Embers Duration", 0.0f, 20.0f, 0.05f),
        SERIALIZED_BOOL(m_debugPlayTransformation, "DEBUG - Play Transformation")
    )
)

AelorinParticles::AelorinParticles(GameObject* owner)
    : Script(owner)
{
}

void AelorinParticles::Start()
{
    m_ownerTransform = GameObjectAPI::getTransform(getOwner());
}

void AelorinParticles::OnGameStop()
{
    releaseRuntimeParticles();
}

void AelorinParticles::releaseRuntimeParticles()
{
    m_timedEffects.clear();
    m_timedOneShots.clear();

    for (GameObject*& instance : m_soulPillarInstances)
    {
        ParticleLifecycle::destroy(instance);
    }
    m_soulPillarInstances.clear();
    ParticleLifecycle::destroy(m_soulsAuraInstance);
    ParticleLifecycle::destroy(m_turningEmbers1Instance);
    ParticleLifecycle::destroy(m_turningEmbers2Instance);
    ParticleLifecycle::destroy(m_phase2AuraInstance);

    m_phase2AuraActive = false;
}

void AelorinParticles::Update()
{
    if (m_debugPlayTransformation)
    {
        m_debugPlayTransformation = false;
        playPhaseTransformation();
    }

    const float deltaTime = Time::getDeltaTime();

    processTimedEffects(deltaTime);
    m_timedOneShots.update(deltaTime);

    if (m_phase2AuraActive && m_phase2AuraInstance != nullptr)
    {
        ParticleLifecycle::syncToTransform(m_phase2AuraInstance, m_ownerTransform);
    }
}

void AelorinParticles::playPhaseTransformation()
{
    startSoulPillar();

    scheduleEffect(TimedEffect::Type::Shockwave1, m_shockwave1Delay);
    scheduleEffect(TimedEffect::Type::Shockwave2, m_shockwave2Delay);
    scheduleEffect(TimedEffect::Type::StopPillar, m_soulPillarDuration);
}

void AelorinParticles::startSoulPillar()
{
    const Vector3 position = getBossPosition() + m_soulPillarOffset;

    // El prefab de la columna es una sola particula alargada: en anillo forma el cilindro.
    const int count = m_soulPillarCount > 0 ? m_soulPillarCount : 1;
    m_soulPillarInstances.resize(static_cast<size_t>(count), nullptr);

    for (int i = 0; i < count; ++i)
    {
        const float angle = (MathAPI::TWO_PI * static_cast<float>(i)) / static_cast<float>(count);
        const Vector3 ringOffset(std::cos(angle) * m_soulPillarRadius, 0.0f, std::sin(angle) * m_soulPillarRadius);

        GameObject*& instance = m_soulPillarInstances[static_cast<size_t>(i)];
        ParticleLifecycle::ensurePersistent(instance, m_soulPillarPrefab.m_id, position + ringOffset, Vector3::Zero, getOwner());

        if (instance != nullptr)
        {
            Transform* instanceTransform = GameObjectAPI::getTransform(instance);
            if (instanceTransform != nullptr)
            {
                TransformAPI::setGlobalPosition(instanceTransform, position + ringOffset);
                TransformAPI::setScale(instanceTransform, m_soulPillarScale);
            }

            ParticleLifecycle::activate(instance);
        }
    }

    ParticleLifecycle::ensurePersistent(m_soulsAuraInstance, m_soulsAuraPrefab.m_id, position, Vector3::Zero, getOwner());
    ParticleLifecycle::ensurePersistent(m_turningEmbers1Instance, m_turningEmbers1Prefab.m_id, position, Vector3::Zero, getOwner());
    ParticleLifecycle::ensurePersistent(m_turningEmbers2Instance, m_turningEmbers2Prefab.m_id, position, Vector3::Zero, getOwner());

    applyPillarTransform();

    ParticleLifecycle::activate(m_soulsAuraInstance);
    ParticleLifecycle::activateTimed(m_timedOneShots, m_turningEmbers1Instance, m_embersDuration);
    ParticleLifecycle::activateTimed(m_timedOneShots, m_turningEmbers2Instance, m_embersDuration);
}

void AelorinParticles::stopSoulPillar()
{
    for (GameObject* instance : m_soulPillarInstances)
    {
        ParticleLifecycle::deactivate(instance);
    }

    ParticleLifecycle::deactivate(m_soulsAuraInstance);
}

void AelorinParticles::startPhase2Aura()
{
    const Vector3 position = getBossPosition();

    ParticleLifecycle::ensurePersistent(m_phase2AuraInstance, m_phase2AuraPrefab.m_id, position, Vector3::Zero, getOwner());
    ParticleLifecycle::syncToTransform(m_phase2AuraInstance, m_ownerTransform);
    ParticleLifecycle::activate(m_phase2AuraInstance);

    m_phase2AuraActive = m_phase2AuraInstance != nullptr;
}

void AelorinParticles::stopPhase2Aura()
{
    ParticleLifecycle::deactivate(m_phase2AuraInstance);
    m_phase2AuraActive = false;
}

void AelorinParticles::scheduleEffect(TimedEffect::Type type, float delay)
{
    TimedEffect effect;
    effect.type = type;
    effect.timer = delay;
    m_timedEffects.push_back(effect);
}

void AelorinParticles::processTimedEffects(float deltaTime)
{
    for (size_t i = m_timedEffects.size(); i-- > 0;)
    {
        TimedEffect& effect = m_timedEffects[i];

        effect.timer -= deltaTime;

        if (effect.timer > 0.0f)
        {
            continue;
        }

        switch (effect.type)
        {
        case TimedEffect::Type::Shockwave1:
            spawnShockwave(m_turningShockwave1Prefab);
            break;
        case TimedEffect::Type::Shockwave2:
            spawnShockwave(m_turningShockwave2Prefab);
            break;
        case TimedEffect::Type::StopPillar:
            stopSoulPillar();
            break;
        }

        m_timedEffects.erase(m_timedEffects.begin() + static_cast<std::ptrdiff_t>(i));
    }
}

void AelorinParticles::spawnShockwave(const PrefabRef& prefab)
{
    ParticleLifecycle::spawnOneShotTimed(
        m_timedOneShots,
        prefab.m_id,
        getBossPosition(),
        Vector3::Zero,
        ParticleLifecycle::kDefaultOneShotLifetime
    );
}

Vector3 AelorinParticles::getBossPosition() const
{
    if (m_ownerTransform == nullptr)
    {
        return Vector3::Zero;
    }

    return TransformAPI::getGlobalPosition(m_ownerTransform);
}

void AelorinParticles::applyPillarTransform()
{
    const Vector3 position = getBossPosition() + m_soulPillarOffset;

    GameObject* instances[] =
    {
        m_soulsAuraInstance,
        m_turningEmbers1Instance,
        m_turningEmbers2Instance
    };

    for (GameObject* instance : instances)
    {
        if (instance == nullptr)
        {
            continue;
        }

        Transform* instanceTransform = GameObjectAPI::getTransform(instance);
        if (instanceTransform != nullptr)
        {
            TransformAPI::setGlobalPosition(instanceTransform, position);
        }
    }
}

IMPLEMENT_SCRIPT(AelorinParticles)

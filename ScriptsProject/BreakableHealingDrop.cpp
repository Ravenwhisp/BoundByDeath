#include "pch.h"
#include "BreakableHealingDrop.h"

#include "HealthDropSpawner.h"
#include "EnvironmentSound.h"
#include "ParticleLifecycle.h"

IMPLEMENT_SCRIPT_FIELDS_INHERITED(BreakableHealingDrop, BreakableObject,
    SERIALIZED_ASSET_REF(m_healthPickupPrefab, "Health Pickup Prefab", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_healthBreakEffectParticle, "Health Break Effect Particle", AssetType::PREFAB),
    SERIALIZED_FLOAT(m_healthDropAmount, "Health Drop Amount", 0.0f, 100.0f, 1.0f),
    SERIALIZED_FLOAT(m_dropRadius, "Drop Radius", 0.0f, 5.0f, 0.1f),
    SERIALIZED_FLOAT(m_dropHeight, "Drop Height", 0.0f, 5.0f, 0.1f),
    SERIALIZED_INT(m_healthDropQuantity, "Health Drop Quantity")
)

BreakableHealingDrop::BreakableHealingDrop(GameObject* owner)
    : BreakableObject(owner)
{
}

void BreakableHealingDrop::Start()
{
    BreakableObject::Start();
}

void BreakableHealingDrop::OnGameStop()
{
    ParticleLifecycle::destroy(m_healthBreakEffectInstance);
    BreakableObject::OnGameStop();
}

void BreakableHealingDrop::ensureHealthBreakEffect()
{
    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    const Vector3 spawnPosition = ownerTransform != nullptr ? TransformAPI::getGlobalPosition(ownerTransform) : Vector3::Zero;
    ParticleLifecycle::ensurePersistent(m_healthBreakEffectInstance, m_healthBreakEffectParticle.m_id, spawnPosition, Vector3::Zero, nullptr);
}

void BreakableHealingDrop::Update()
{
}

void BreakableHealingDrop::onBreak()
{
    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());

    const Vector3 breakablePosition = TransformAPI::getGlobalPosition(ownerTransform);

    if (m_healthPickupPrefab.m_id.isValid())
    {
        for (int i = 0; i < m_healthDropQuantity; ++i)
        {
            HealthDropSpawner::drop(m_healthPickupPrefab.m_id, breakablePosition, m_healthDropAmount, m_dropRadius, m_dropHeight);
        }
    }
    else
    {
        Debug::warn("[BreakableHealingDrop] '%s' has no health pickup prefab set. Breaking without spawning health.", GameObjectAPI::getName(getOwner()));
    }

    if (m_healthBreakEffectParticle.m_id.isValid())
    {
        ensureHealthBreakEffect();
        if (m_healthBreakEffectInstance != nullptr)
        {
            Transform* effectTransform = GameObjectAPI::getTransform(m_healthBreakEffectInstance);
            if (effectTransform != nullptr)
            {
                TransformAPI::setGlobalPosition(effectTransform, breakablePosition);
            }
            ParticleLifecycle::activate(m_healthBreakEffectInstance);
        }
    }

    // It's still a barrel/crate breaking → same break SFX.
    EnvironmentSound::play(getOwner(), "Play_Environment_Barrel_Break");

    BreakableObject::breakObject();
}

IMPLEMENT_SCRIPT(BreakableHealingDrop)
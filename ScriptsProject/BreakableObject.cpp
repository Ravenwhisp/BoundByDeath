#include "pch.h"
#include "BreakableObject.h"

#include "EnvironmentSound.h"
#include "ParticleLifecycle.h"

namespace
{
    constexpr const char* k_barrelBreak = "Play_Environment_Barrel_Break";
}

IMPLEMENT_SCRIPT_FIELDS(BreakableObject,
    SERIALIZED_ASSET_REF(m_dustEffectParticle, "Dust Effect Particle", AssetType::PREFAB)
)

BreakableObject::BreakableObject(GameObject* owner)
    : Script(owner)
{
}

void BreakableObject::Start()
{
    m_isBroken = false;

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());

    m_normalObjectTransform = TransformAPI::findChildByName(ownerTransform, "Normal");
    m_brokenObjectTransform = TransformAPI::findChildByName(ownerTransform, "Broken");

    if (m_normalObjectTransform == nullptr)
    {
        Debug::warn("[BreakableObject] '%s' could not find child object named 'Normal'.", GameObjectAPI::getName(getOwner()));
    }

    if (m_brokenObjectTransform == nullptr)
    {
        Debug::warn("[BreakableObject] '%s' could not find child object named 'Broken'.", GameObjectAPI::getName(getOwner()));
    }

    if (m_normalObjectTransform != nullptr)
    {
        GameObject* normalObject = ComponentAPI::getOwner(m_normalObjectTransform);
        GameObjectAPI::setActive(normalObject, true);
    }

    if (m_brokenObjectTransform != nullptr)
    {
        GameObject* brokenObject = ComponentAPI::getOwner(m_brokenObjectTransform);
        GameObjectAPI::setActive(brokenObject, false);
    }

    m_navBlocker = NavigationAPI::getRuntimeBlockerComponent(getOwner());
    if (m_navBlocker != nullptr)
    {
        NavigationAPI::setBlocked(m_navBlocker, true);
    }
}

void BreakableObject::OnGameStop()
{
    ParticleLifecycle::destroy(m_dustEffectInstance);
}

void BreakableObject::ensureDustEffect()
{
    Vector3 spawnPosition = Vector3::Zero;
    if (m_brokenObjectTransform != nullptr)
    {
        spawnPosition = TransformAPI::getGlobalPosition(m_brokenObjectTransform);
    }
    else
    {
        Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
        if (ownerTransform != nullptr)
        {
            spawnPosition = TransformAPI::getGlobalPosition(ownerTransform);
        }
    }

    ParticleLifecycle::ensurePersistent(m_dustEffectInstance, m_dustEffectParticle.m_id, spawnPosition, Vector3::Zero, nullptr);
}

void BreakableObject::onBreak()
{
    breakObject();
    EnvironmentSound::play(getOwner(), k_barrelBreak);   // barrels & crates (same prefab)
}

void BreakableObject::breakObject()
{
    if (m_isBroken)
    {
        return;
    }

    m_isBroken = true;

    if (m_normalObjectTransform != nullptr)
    {
        GameObject* normalObject = ComponentAPI::getOwner(m_normalObjectTransform);
        GameObjectAPI::setActive(normalObject, false);
    }

    if (m_brokenObjectTransform != nullptr)
    {
        GameObject* brokenObject = ComponentAPI::getOwner(m_brokenObjectTransform);
        GameObjectAPI::setActive(brokenObject, true);

        ensureDustEffect();
        if (m_dustEffectInstance != nullptr)
        {
            Transform* dustTransform = GameObjectAPI::getTransform(m_dustEffectInstance);
            if (dustTransform != nullptr)
            {
                TransformAPI::setGlobalPosition(dustTransform, TransformAPI::getGlobalPosition(m_brokenObjectTransform));
            }
            ParticleLifecycle::activate(m_dustEffectInstance);
        }
    }

    if (m_navBlocker != nullptr)
    {
        NavigationAPI::setBlocked(m_navBlocker, false);
    }

    Debug::log("[BreakableObject] '%s' broke.", GameObjectAPI::getName(getOwner()));
}

IMPLEMENT_SCRIPT(BreakableObject)
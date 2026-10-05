#include "pch.h"
#include "LyrielParticles.h"
#include "ParticleLifecycle.h"

IMPLEMENT_SCRIPT_FIELDS(LyrielParticles,
    SERIALIZED_COMPONENT_REF(m_dashTrail, "Dash", ComponentType::TRANSFORM),
    SERIALIZED_STRING(m_chargeGlowPath, "Charge Glow Prefab Path"),
    SERIALIZED_ASSET_REF(m_chargeGlowPrefab, "Charge Glow Prefab", AssetType::PREFAB),
    SERIALIZED_STRING(m_dashParticlePath, "Dash Particle Prefab Path"),
    SERIALIZED_ASSET_REF(m_dashParticlePrefab, "Dash Particle Prefab", AssetType::PREFAB),
    SERIALIZED_STRING(m_hitFlashPath, "Hit Flash Prefab Path"),
    SERIALIZED_ASSET_REF(m_hitFlashPrefab, "Hit Flash Prefab", AssetType::PREFAB),
    SERIALIZED_STRING(m_bowAnchorName, "Bow Anchor Name"),
    SERIALIZED_INT(m_hitFlashPoolSize, "Hit Flash Pool Size")
)

LyrielParticles::LyrielParticles(GameObject* owner)
    : Script(owner)
{
}

void LyrielParticles::Start()
{
    SetDashInactive();

    Transform* bowTransform = findBowTransform();
    const Vector3 position = bowTransform != nullptr ? TransformAPI::getGlobalPosition(bowTransform) : Vector3::Zero;
    const Vector3 rotation = bowTransform != nullptr ? TransformAPI::getGlobalEulerDegrees(bowTransform) : Vector3::Zero;
    ParticleLifecycle::ensurePersistent(m_chargeGlowInstance, m_chargeGlowPrefab.m_id, position, rotation, getOwner());
    ParticleLifecycle::deactivate(m_chargeGlowInstance);

    prewarmHitFlashes();
}

void LyrielParticles::OnGameStop()
{
    ParticleLifecycle::destroy(m_chargeGlowInstance);
    ParticleLifecycle::destroy(m_dashParticleInstance);
    for (HitFlashSlot& slot : m_hitFlashPool)
    {
        ParticleLifecycle::destroy(slot.instance);
    }
    m_hitFlashPool.clear();
    m_chargeGlowActive = false;
    m_dashParticleActive = false;
}

void LyrielParticles::Update()
{
    updateHitFlashes(Time::getDeltaTime());
    syncActiveParticles();
}

Transform* LyrielParticles::getTransform(ComponentRef<Transform> controller)
{
    return controller.getReferencedComponent();
}

Transform* LyrielParticles::findBowTransform() const
{
    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    if (ownerTransform == nullptr)
    {
        return nullptr;
    }

    if (Transform* bow = ParticleLifecycle::findChildRecursive(ownerTransform, m_bowAnchorName.c_str()))
    {
        return bow;
    }

    if (Transform* arbalet = ParticleLifecycle::findChildRecursive(ownerTransform, "arbalet"))
    {
        return arbalet;
    }

    return ownerTransform;
}

void LyrielParticles::syncActiveParticles()
{
    if (m_chargeGlowActive)
    {
        ParticleLifecycle::syncToTransform(m_chargeGlowInstance, findBowTransform());
    }

    if (m_dashParticleActive)
    {
        ParticleLifecycle::syncToTransform(m_dashParticleInstance, GameObjectAPI::getTransform(getOwner()));
    }
}

void LyrielParticles::SetDashActive()
{
    if (m_dashTrailController == nullptr)
    {
        m_dashTrailController = getTransform(m_dashTrail);
    }

    if (m_dashTrailController != nullptr)
    {
        const int childCount = TransformAPI::getChildCount(m_dashTrailController);
        for (int i = 0; i < childCount; ++i)
        {
            Transform* child = TransformAPI::getChild(m_dashTrailController, i);
            TrailComponent* trailComponent = TrailAPI::getTrailComponent(ComponentAPI::getOwner(child));
            TrailAPI::generateTrail(trailComponent, true);
        }
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    const Vector3 position = ownerTransform != nullptr ? TransformAPI::getGlobalPosition(ownerTransform) : Vector3::Zero;
    const Vector3 rotation = ownerTransform != nullptr ? TransformAPI::getGlobalEulerDegrees(ownerTransform) : Vector3::Zero;

    ParticleLifecycle::ensurePersistent(m_dashParticleInstance, m_dashParticlePrefab.m_id, position, rotation, getOwner());
    ParticleLifecycle::syncToTransform(m_dashParticleInstance, ownerTransform);
    ParticleLifecycle::activate(m_dashParticleInstance);
    m_dashParticleActive = m_dashParticleInstance != nullptr;
}

void LyrielParticles::SetDashInactive()
{
    if (m_dashTrailController == nullptr)
    {
        m_dashTrailController = getTransform(m_dashTrail);
    }

    if (m_dashTrailController != nullptr)
    {
        const int childCount = TransformAPI::getChildCount(m_dashTrailController);
        for (int i = 0; i < childCount; ++i)
        {
            Transform* child = TransformAPI::getChild(m_dashTrailController, i);
            TrailComponent* trailComponent = TrailAPI::getTrailComponent(ComponentAPI::getOwner(child));
            TrailAPI::generateTrail(trailComponent, false);
        }
    }

    ParticleLifecycle::deactivate(m_dashParticleInstance);
    m_dashParticleActive = false;
}

void LyrielParticles::SetChargeActive()
{
    Transform* bowTransform = findBowTransform();
    const Vector3 position = bowTransform != nullptr ? TransformAPI::getGlobalPosition(bowTransform) : Vector3::Zero;
    const Vector3 rotation = bowTransform != nullptr ? TransformAPI::getGlobalEulerDegrees(bowTransform) : Vector3::Zero;

    ParticleLifecycle::ensurePersistent(m_chargeGlowInstance, m_chargeGlowPrefab.m_id, position, rotation, getOwner());
    ParticleLifecycle::syncToTransform(m_chargeGlowInstance, bowTransform);
    ParticleLifecycle::activate(m_chargeGlowInstance);
    m_chargeGlowActive = m_chargeGlowInstance != nullptr;
}

void LyrielParticles::SetChargeInactive()
{
    ParticleLifecycle::deactivate(m_chargeGlowInstance);
    m_chargeGlowActive = false;
}

void LyrielParticles::playHitFlash(const Vector3& position, GameObject* target)
{
    HitFlashSlot* selected = nullptr;
    for (HitFlashSlot& slot : m_hitFlashPool)
    {
        if (slot.remainingSeconds <= 0.0f)
        {
            selected = &slot;
            break;
        }

        if (selected == nullptr || slot.remainingSeconds < selected->remainingSeconds)
        {
            selected = &slot;
        }
    }

    if (selected == nullptr || selected->instance == nullptr)
    {
        return;
    }

    Transform* effectTransform = GameObjectAPI::getTransform(selected->instance);
    if (effectTransform != nullptr)
    {
        TransformAPI::setGlobalPosition(effectTransform, position);
        TransformAPI::setGlobalRotationEuler(effectTransform, Vector3::Zero);
    }

    ParticleLifecycle::activate(selected->instance);
    selected->target = target;
    selected->targetOffset = Vector3::Zero;
    if (target != nullptr)
    {
        Transform* targetTransform = GameObjectAPI::getTransform(target);
        if (targetTransform != nullptr)
        {
            selected->targetOffset = position - TransformAPI::getGlobalPosition(targetTransform);
        }
    }
    selected->remainingSeconds = ParticleLifecycle::kDefaultOneShotLifetime;
}

void LyrielParticles::prewarmHitFlashes()
{
    const int poolSize = (std::max)(m_hitFlashPoolSize, 0);
    m_hitFlashPool.clear();
    m_hitFlashPool.reserve(poolSize);

    for (int i = 0; i < poolSize; ++i)
    {
        HitFlashSlot slot;
        slot.instance = ParticleLifecycle::instantiatePersistent(
            m_hitFlashPrefab.m_id,
            Vector3::Zero,
            Vector3::Zero,
            ParticleLifecycle::getRuntimeVfxContainer());
        if (slot.instance != nullptr)
        {
            m_hitFlashPool.push_back(slot);
        }
    }
}

void LyrielParticles::updateHitFlashes(float deltaTime)
{
    for (HitFlashSlot& slot : m_hitFlashPool)
    {
        if (slot.remainingSeconds <= 0.0f)
        {
            continue;
        }

        if (slot.target != nullptr && SceneAPI::containsGameObject(slot.target))
        {
            Transform* effectTransform = GameObjectAPI::getTransform(slot.instance);
            Transform* targetTransform = GameObjectAPI::getTransform(slot.target);
            if (effectTransform != nullptr && targetTransform != nullptr)
            {
                TransformAPI::setGlobalPosition(
                    effectTransform,
                    TransformAPI::getGlobalPosition(targetTransform) + slot.targetOffset);
            }
        }
        else
        {
            slot.target = nullptr;
        }

        slot.remainingSeconds -= deltaTime;
        if (slot.remainingSeconds <= 0.0f)
        {
            slot.remainingSeconds = 0.0f;
            slot.target = nullptr;
            ParticleLifecycle::deactivate(slot.instance);
        }
    }
}

IMPLEMENT_SCRIPT(LyrielParticles)

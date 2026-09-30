#include "pch.h"
#include "DeathParticles.h"
#include "ParticleLifecycle.h"
#include "EnemyDamageable.h"
#include "EnemyForcedMovement.h"

#include <algorithm>
#include <cmath>
#include <cstring>

IMPLEMENT_SCRIPT_FIELDS(DeathParticles,
    SERIALIZED_COMPONENT_REF(m_dashTrail, "Dash", ComponentType::TRANSFORM),
    SERIALIZED_COMPONENT_REF(m_scytheTrail, "Scythe", ComponentType::TRANSFORM),
    SERIALIZED_STRING(m_tauntParticlePath, "Taunt Particle Prefab Path"),
    SERIALIZED_ASSET_REF(m_tauntParticle, "Taunt Particle Prefab", AssetType::PREFAB),
    SERIALIZED_STRING(m_dashParticlePath, "Dash Particle Prefab Path"),
    SERIALIZED_ASSET_REF(m_dashParticlePrefab, "Dash Particle Prefab", AssetType::PREFAB),
    SERIALIZED_STRING(m_chargeGlowPath, "Charge Glow Prefab Path"),
    SERIALIZED_ASSET_REF(m_chargeGlowPrefab, "Charge Glow Prefab", AssetType::PREFAB),
    SERIALIZED_STRING(m_hitFlashPath, "Hit Flash Prefab Path"),
    SERIALIZED_ASSET_REF(m_hitFlashPrefab, "Hit Flash Prefab", AssetType::PREFAB),
    SERIALIZED_STRING(m_chargedHitFlashPath, "Charged Hit Flash Pzrefab Path"),
    SERIALIZED_ASSET_REF(m_chargedHitFlashPrefab, "Charged Hit Flash Prefab", AssetType::PREFAB),
    SERIALIZED_STRING(m_scytheAnchorName, "Scythe Anchor Name"),
    SERIALIZED_ASSET_REF(m_tauntChainLinkPrefab, "Taunt Chain Link Prefab (optional)", AssetType::PREFAB),
    SERIALIZED_STRING(m_tauntChainHandBone, "Taunt Chain Hand Bone"),
    SERIALIZED_BOOL(m_tauntChainOnFloor, "Taunt Chain On Floor"),
    SERIALIZED_FLOAT(m_tauntChainFloorHeight, "Taunt Chain Floor Height", 0.0f, 0.5f, 0.01f),
    SERIALIZED_FLOAT(m_tauntChainStartOffset, "Taunt Chain Start Offset", 0.0f, 2.0f, 0.05f),
    SERIALIZED_FLOAT(m_tauntChainTipHeight, "Taunt Chain Tip Height", 0.0f, 3.0f, 0.05f),
    SERIALIZED_FLOAT(m_tauntChainLinkSpacing, "Taunt Chain Link Spacing", 0.05f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_tauntChainLinkScale, "Taunt Chain Link Scale", 0.1f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_tauntChainMaxLength, "Taunt Chain Max Length", 1.0f, 20.0f, 0.5f),
    SERIALIZED_INT(m_tauntChainMaxChains, "Taunt Chain Max Chains"),
    SERIALIZED_FLOAT(m_tauntChainTravelTime, "Taunt Chain Travel Time", 0.02f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_tauntChainMinLatchTime, "Taunt Chain Min Latch Time", 0.0f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_tauntChainMaxLatchTime, "Taunt Chain Max Latch Time", 0.1f, 3.0f, 0.05f),
    SERIALIZED_FLOAT(m_tauntChainRetractTime, "Taunt Chain Retract Time", 0.02f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_tauntChainWhip, "Taunt Chain Whip", 0.0f, 2.0f, 0.01f),
    SERIALIZED_FLOAT(m_tauntChainTension, "Taunt Chain Tension Shake", 0.0f, 0.3f, 0.005f)
)

DeathParticles::DeathParticles(GameObject* owner)
    : Script(owner)
{
}

void DeathParticles::Start()
{
    SetDashInactive();
    SetScytheInactive();
}

void DeathParticles::OnGameStop()
{
    ParticleLifecycle::destroy(m_activeTauntParticle);
    ParticleLifecycle::destroy(m_dashParticleInstance);
    ParticleLifecycle::destroy(m_chargeGlowInstance);
    m_timedOneShots.clear();
    m_tauntParticleActive = false;
    m_dashParticleActive = false;
    m_chargeGlowActive = false;
    m_tauntParticleLifetime = 0.0f;

    destroyTauntChainPool();
}

void DeathParticles::Update()
{
    m_timedOneShots.update(Time::getDeltaTime());
    syncActiveParticles();

    if (!m_tauntChainPoolBuilt)
    {
        ensureTauntChainPool(); // spawn the link pool up front so the first taunt doesn't hitch
    }
    updateTauntChains(Time::getDeltaTime());

    if (!m_tauntParticleActive)
    {
        return;
    }

    m_tauntParticleLifetime -= Time::getDeltaTime();

    if (m_tauntParticleLifetime <= 0.0f)
    {
        SetTauntInactive();
    }
}

Transform* DeathParticles::getTransform(ComponentRef<Transform> controller)
{
    return controller.getReferencedComponent();
}

Transform* DeathParticles::findScytheTransform() const
{
    if (Transform* scytheTrail = m_scytheTrail.getReferencedComponent())
    {
        return scytheTrail;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    if (ownerTransform == nullptr)
    {
        return nullptr;
    }

    if (Transform* named = ParticleLifecycle::findChildRecursive(ownerTransform, m_scytheAnchorName.c_str()))
    {
        return named;
    }

    if (Transform* trail = ParticleLifecycle::findChildRecursive(ownerTransform, "scythe_trail_controller"))
    {
        return trail;
    }

    return ownerTransform;
}

void DeathParticles::syncActiveParticles()
{
    if (m_chargeGlowActive)
    {
        ParticleLifecycle::syncToTransform(m_chargeGlowInstance, findScytheTransform());
    }

    if (m_dashParticleActive)
    {
        ParticleLifecycle::syncToTransform(m_dashParticleInstance, GameObjectAPI::getTransform(getOwner()));
    }
}

void DeathParticles::ensureTauntParticle(const Vector3& position, const Vector3& rotation)
{
    ParticleLifecycle::ensurePersistent(m_activeTauntParticle, m_tauntParticle.m_id, position, rotation, nullptr);
}

void DeathParticles::SetDashActive()
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

    ParticleLifecycle::ensurePersistent(m_dashParticleInstance, m_dashParticlePrefab.m_id, position, rotation, nullptr);
    ParticleLifecycle::syncToTransform(m_dashParticleInstance, ownerTransform);
    ParticleLifecycle::activate(m_dashParticleInstance);
    m_dashParticleActive = m_dashParticleInstance != nullptr;
}

void DeathParticles::SetDashInactive()
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

void DeathParticles::SetScytheActive()
{
    if (m_scytheTrailController == nullptr)
    {
        m_scytheTrailController = getTransform(m_scytheTrail);
    }

    if (m_scytheTrailController == nullptr)
    {
        return;
    }

    TrailComponent* trailComponent = TrailAPI::getTrailComponent(ComponentAPI::getOwner(m_scytheTrailController));
    TrailAPI::generateTrail(trailComponent, true);
}

void DeathParticles::SetScytheInactive()
{
    if (m_scytheTrailController == nullptr)
    {
        m_scytheTrailController = getTransform(m_scytheTrail);
    }

    if (m_scytheTrailController == nullptr)
    {
        return;
    }

    TrailComponent* trailComponent = TrailAPI::getTrailComponent(ComponentAPI::getOwner(m_scytheTrailController));
    TrailAPI::generateTrail(trailComponent, false);
}

void DeathParticles::SetChargeActive()
{
    Transform* scytheTransform = findScytheTransform();
    const Vector3 position = scytheTransform != nullptr ? TransformAPI::getGlobalPosition(scytheTransform) : Vector3::Zero;
    const Vector3 rotation = scytheTransform != nullptr ? TransformAPI::getGlobalEulerDegrees(scytheTransform) : Vector3::Zero;

    ParticleLifecycle::ensurePersistent(m_chargeGlowInstance, m_chargeGlowPrefab.m_id, position, rotation, nullptr);
    ParticleLifecycle::syncToTransform(m_chargeGlowInstance, scytheTransform);
    ParticleLifecycle::activate(m_chargeGlowInstance);
    m_chargeGlowActive = m_chargeGlowInstance != nullptr;
}

void DeathParticles::SetChargeInactive()
{
    ParticleLifecycle::deactivate(m_chargeGlowInstance);
    m_chargeGlowActive = false;
}

void DeathParticles::SetTauntActive(const Vector3& direction)
{
    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    if (ownerTransform == nullptr)
    {
        Debug::warn("[DeathParticles] Owner transform not found.");
        return;
    }

    Vector3 spawnPosition = TransformAPI::getGlobalPosition(ownerTransform);

    Vector3 flatDirection = direction;
    flatDirection.y = 0.0f;

    if (flatDirection.LengthSquared() <= 0.0001f)
    {
        Debug::warn("[DeathParticles] Invalid taunt direction.");
        return;
    }

    flatDirection.Normalize();

    const float yawRad = std::atan2(flatDirection.x, flatDirection.z);
    const float yawDeg = yawRad * (180.0f / 3.14159265f);

    Vector3 particleRootRotation(0.0f, yawDeg, 0.0f);

    ensureTauntParticle(spawnPosition, particleRootRotation);

    if (m_activeTauntParticle == nullptr)
    {
        Debug::warn("[DeathParticles] Could not instantiate taunt particle.");
        return;
    }

    Transform* particleTransform = GameObjectAPI::getTransform(m_activeTauntParticle);
    if (particleTransform != nullptr)
    {
        TransformAPI::setGlobalPosition(particleTransform, spawnPosition);
        TransformAPI::setGlobalRotationEuler(particleTransform, particleRootRotation);
    }

    ParticleLifecycle::activate(m_activeTauntParticle);
    m_tauntParticleActive = true;
    m_tauntParticleLifetime = 1.0f;
}

void DeathParticles::SetTauntInactive()
{
    ParticleLifecycle::deactivate(m_activeTauntParticle);
    m_tauntParticleActive = false;
    m_tauntParticleLifetime = 0.0f;
}

void DeathParticles::playHitFlash(const Vector3& position)
{
    ParticleLifecycle::spawnOneShotTimed(
        m_timedOneShots,
        m_hitFlashPrefab.m_id,
        position
    );
}

void DeathParticles::playChargedHitFlash(const Vector3& position)
{
    ParticleLifecycle::spawnOneShotTimed(
        m_timedOneShots,
        m_chargedHitFlashPrefab.m_id,
        position
    );
}

namespace
{
    float clamp01(float value)
    {
        return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
    }
}

namespace
{
    constexpr UID kDefaultChainLinkPrefabUid = 5894923481231074528ull;
    constexpr float kRadToDeg = 180.0f / 3.14159265f;
}

void DeathParticles::ensureTauntChainPool()
{
    if (m_tauntChainPoolBuilt)
    {
        return;
    }

    m_tauntChainPoolBuilt = true;

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    m_tauntChainHandBoneTransform = ParticleLifecycle::findChildRecursive(ownerTransform, m_tauntChainHandBone.c_str());

    const AssetId linkPrefab = m_tauntChainLinkPrefab.m_id.hasUID() ? m_tauntChainLinkPrefab.m_id : AssetId(kDefaultChainLinkPrefabUid);

    const int chainCount = m_tauntChainMaxChains > 0 ? m_tauntChainMaxChains : 1;
    const float spacing = m_tauntChainLinkSpacing > 0.01f ? m_tauntChainLinkSpacing : 0.01f;
    const int linksPerChain = static_cast<int>(m_tauntChainMaxLength / spacing) + 2;

    // Everything hangs off one holder so the hierarchy stays tidy.
    m_tauntChainRoot = GameObjectAPI::createGameObject("TauntChains");

    for (int c = 0; c < chainCount; ++c)
    {
        TauntChain chain;
        chain.phase = static_cast<float>(c) * 2.1f;

        for (int i = 0; i < linksPerChain; ++i)
        {
            GameObject* link = GameObjectAPI::instantiatePrefab(linkPrefab, Vector3::Zero, Vector3::Zero, m_tauntChainRoot);
            if (!link)
            {
                Debug::warn("[DeathParticles] Could not spawn the taunt chain link model. Taunt will play without chains.");
                destroyTauntChainPool();
                m_tauntChainPoolBuilt = true; // don't retry every taunt
                return;
            }

            TransformAPI::setScale(GameObjectAPI::getTransform(link), Vector3(m_tauntChainLinkScale, m_tauntChainLinkScale, m_tauntChainLinkScale));
            GameObjectAPI::setActive(link, false);

            chain.links.push_back(link);
            chain.linkVisible.push_back(false);
        }

        m_tauntChains.push_back(chain);
    }
}

void DeathParticles::destroyTauntChainPool()
{
    m_tauntChains.clear();

    if (m_tauntChainRoot && SceneAPI::containsGameObject(m_tauntChainRoot))
    {
        GameObjectAPI::removeGameObject(m_tauntChainRoot);
    }

    m_tauntChainRoot = nullptr;
    m_tauntChainHandBoneTransform = nullptr;
    m_tauntChainPoolBuilt = false;
}

void DeathParticles::launchTauntChains(const std::vector<GameObject*>& targets, float travelTime)
{
    ensureTauntChainPool();

    if (m_tauntChains.empty() || targets.empty())
    {
        return;
    }

    // Nearest enemies get the chains first.
    const Vector3 origin = getTauntChainStart();
    std::vector<GameObject*> sorted = targets;
    std::sort(sorted.begin(), sorted.end(), [&](GameObject* a, GameObject* b)
    {
        const Vector3 da = TransformAPI::getGlobalPosition(GameObjectAPI::getTransform(a)) - origin;
        const Vector3 db = TransformAPI::getGlobalPosition(GameObjectAPI::getTransform(b)) - origin;
        return da.LengthSquared() < db.LengthSquared();
    });

    size_t chainIndex = 0;
    for (GameObject* target : sorted)
    {
        if (!isTauntChainTargetValid(target))
        {
            continue;
        }

        while (chainIndex < m_tauntChains.size() && m_tauntChains[chainIndex].state != TauntChainState::Idle)
        {
            ++chainIndex;
        }

        if (chainIndex >= m_tauntChains.size())
        {
            break;
        }

        TauntChain& chain = m_tauntChains[chainIndex];
        chain.target = target;
        chain.state = TauntChainState::Shooting;
        chain.timer = 0.0f;
        chain.travelTime = travelTime > 0.01f ? travelTime : 0.01f;
        chain.sawPull = false;
        chain.tip = origin;
    }
}

void DeathParticles::cancelTauntChains()
{
    for (TauntChain& chain : m_tauntChains)
    {
        if (chain.state == TauntChainState::Shooting || chain.state == TauntChainState::Latched)
        {
            startTauntChainRetract(chain);
        }
    }
}

void DeathParticles::updateTauntChains(float deltaTime)
{
    if (m_tauntChains.empty())
    {
        return;
    }

    m_tauntChainClock += deltaTime;
    const Vector3 start = getTauntChainStart();

    for (TauntChain& chain : m_tauntChains)
    {
        if (chain.state == TauntChainState::Idle)
        {
            continue;
        }

        chain.timer += deltaTime;
        float whip = 0.0f;

        switch (chain.state)
        {
        case TauntChainState::Shooting:
        {
            if (!isTauntChainTargetValid(chain.target))
            {
                startTauntChainRetract(chain);
                break;
            }

            // Fast out; the whip settles as it arrives.
            const float t = clamp01(chain.timer / chain.travelTime);
            const float eased = MathAPI::evaluateEasing(MathAPI::EasingType::EaseOutQuad, t);
            const Vector3 target = getTauntChainTargetPoint(chain.target);
            chain.tip = start + (target - start) * eased;
            whip = m_tauntChainWhip * (1.0f - eased);

            if (t >= 1.0f)
            {
                chain.state = TauntChainState::Latched;
                chain.timer = 0.0f;
                playHitFlash(target);
            }
            break;
        }

        case TauntChainState::Latched:
        {
            if (!isTauntChainTargetValid(chain.target))
            {
                startTauntChainRetract(chain);
                break;
            }

            const bool pulled = isTauntChainTargetPulled(chain.target);
            if (pulled)
            {
                chain.sawPull = true;
            }

            // Small sideways shake at the enemy end reads as tension.
            Vector3 tip = getTauntChainTargetPoint(chain.target);
            Vector3 side(start.z - tip.z, 0.0f, tip.x - start.x);
            if (side.LengthSquared() > 0.0001f)
            {
                side.Normalize();
                tip = tip + side * (std::sin(m_tauntChainClock * 55.0f + chain.phase) * m_tauntChainTension);
            }
            chain.tip = tip;

            const bool pullFinished = chain.sawPull && !pulled;
            const bool neverPulled = !chain.sawPull && chain.timer >= m_tauntChainMinLatchTime;

            if (pullFinished || neverPulled || chain.timer >= m_tauntChainMaxLatchTime)
            {
                startTauntChainRetract(chain);
            }
            break;
        }

        case TauntChainState::Retracting:
        {
            // Accelerates back into Death, like it's being reeled in
            const float t = clamp01(chain.timer / m_tauntChainRetractTime);
            const float eased = MathAPI::evaluateEasing(MathAPI::EasingType::EaseInCubic, t);
            chain.tip = chain.retractFrom + (start - chain.retractFrom) * eased;
            whip = m_tauntChainWhip * 0.3f * (1.0f - t);

            if (t >= 1.0f)
            {
                chain.state = TauntChainState::Idle;
                chain.target = nullptr;
                hideTauntChain(chain);
                continue;
            }
            break;
        }

        default:
            break;
        }

        if (chain.state != TauntChainState::Idle)
        {
            layoutTauntChain(chain, start, chain.tip, whip);
        }
    }
}

void DeathParticles::layoutTauntChain(TauntChain& chain, const Vector3& start, const Vector3& tip, float whip)
{
    Vector3 axis = start - tip;           
    const float length = axis.Length();
    if (length > 0.0001f)
    {
        axis = axis * (1.0f / length);
    }
    else
    {
        axis = Vector3(0.0f, 0.0f, 1.0f);
    }

    Vector3 side(-axis.z, 0.0f, axis.x);
    if (side.LengthSquared() > 0.0001f)
    {
        side.Normalize();
    }

    const float spacing = m_tauntChainLinkSpacing * m_tauntChainLinkScale;
    const float kPi = 3.14159265f;

    auto linkPosition = [&](float distanceFromTip) -> Vector3
    {
        const float s = length > 0.0001f ? distanceFromTip / length : 0.0f;      
        const float envelope = std::sin(s * kPi);                           
        const float wave = std::sin(s * kPi * 3.0f - m_tauntChainClock * 22.0f + chain.phase);
        return tip + axis * distanceFromTip + side * (whip * envelope * wave);
    };

    for (size_t i = 0; i < chain.links.size(); ++i)
    {
        GameObject* link = chain.links[i];
        const float distance = (static_cast<float>(i) + 0.5f) * spacing;
        const bool visible = distance <= length;

        if (visible != chain.linkVisible[i])
        {
            GameObjectAPI::setActive(link, visible);
            chain.linkVisible[i] = visible;
        }

        if (!visible)
        {
            continue;
        }

        const Vector3 position = linkPosition(distance);

        Vector3 direction = linkPosition(distance + spacing * 0.5f) - linkPosition(distance - spacing * 0.5f);
        if (direction.LengthSquared() < 0.000001f)
        {
            direction = axis;
        }
        direction.Normalize();

        const float yaw = std::atan2(direction.x, direction.z) * kRadToDeg;
        const float pitch = -std::asin(direction.y < -1.0f ? -1.0f : (direction.y > 1.0f ? 1.0f : direction.y)) * kRadToDeg;
        const float roll = (i % 2 == 0) ? 0.0f : 90.0f;

        Transform* linkTransform = GameObjectAPI::getTransform(link);
        TransformAPI::setGlobalPosition(linkTransform, position);
        TransformAPI::setGlobalRotationEuler(linkTransform, Vector3(pitch, yaw, roll));
    }
}

void DeathParticles::hideTauntChain(TauntChain& chain)
{
    for (size_t i = 0; i < chain.links.size(); ++i)
    {
        if (chain.linkVisible[i])
        {
            GameObjectAPI::setActive(chain.links[i], false);
            chain.linkVisible[i] = false;
        }
    }
}

void DeathParticles::startTauntChainRetract(TauntChain& chain)
{
    chain.state = TauntChainState::Retracting;
    chain.timer = 0.0f;
    chain.retractFrom = chain.tip;
    chain.target = nullptr;
}

Vector3 DeathParticles::getTauntChainStart() const
{
    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    if (!ownerTransform)
    {
        return Vector3::Zero;
    }

    if (!m_tauntChainOnFloor && m_tauntChainHandBoneTransform)
    {
        return TransformAPI::getGlobalPosition(m_tauntChainHandBoneTransform);
    }

    Vector3 forward = TransformAPI::getForward(ownerTransform);
    forward.y = 0.0f;
    if (forward.LengthSquared() > 0.0001f)
    {
        forward.Normalize();
    }

    const float height = m_tauntChainOnFloor ? m_tauntChainFloorHeight : 1.3f;
    return TransformAPI::getGlobalPosition(ownerTransform) + forward * m_tauntChainStartOffset + Vector3(0.0f, height, 0.0f);
}

Vector3 DeathParticles::getTauntChainTargetPoint(GameObject* target) const
{
    Transform* targetTransform = GameObjectAPI::getTransform(target);
    const float height = m_tauntChainOnFloor ? m_tauntChainFloorHeight : m_tauntChainTipHeight;
    return TransformAPI::getGlobalPosition(targetTransform) + Vector3(0.0f, height, 0.0f);
}

bool DeathParticles::isTauntChainTargetValid(GameObject* target) const
{
    if (!target || !SceneAPI::containsGameObject(target))
    {
        return false;
    }

    EnemyDamageable* damageable = GameObjectAPI::findScript<EnemyDamageable>(target);
    return !(damageable && damageable->isDead());
}

bool DeathParticles::isTauntChainTargetPulled(GameObject* target) const
{
    EnemyForcedMovement* forcedMovement = GameObjectAPI::findScript<EnemyForcedMovement>(target);
    return forcedMovement && forcedMovement->isBeingPulled();
}

IMPLEMENT_SCRIPT(DeathParticles)

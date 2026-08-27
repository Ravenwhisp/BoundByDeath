#include "pch.h"
#include "PaladinVFX.h"
#include "ParticleLifecycle.h"

IMPLEMENT_SCRIPT_FIELDS(PaladinVFX,
    SERIALIZED_ASSET_REF(m_walkingDustPrefab, "Walking Dust Prefab", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_chargeAttackEffectPrefab, "Charge Attack Effect Prefab", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_basicAttackEffectPrefab, "Basic Attack Effect Prefab", AssetType::PREFAB),
    SERIALIZED_FLOAT(walkingDustYOffset, "Walking Dust Y Offset", -5.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(walkingDustForwardOffset, "Walking Dust Forward Offset", -5.0f, 5.0f, 0.05f)
)

PaladinVFX::PaladinVFX(GameObject* owner)
    : Script(owner)
{
}

void PaladinVFX::Start()
{
    walkingDustActive = false;
    chargeAttackEffectActive = false;
    basicAttackEffectTimer = 0.0f;
}

void PaladinVFX::OnGameStop()
{
    ParticleLifecycle::destroy(walkingDustEffect);
    ParticleLifecycle::destroy(chargeAttackEffect);
    ParticleLifecycle::destroy(basicAttackTelegraph);
    ParticleLifecycle::destroy(basicAttackEffect);
}

void PaladinVFX::Update()
{
    if (walkingDustActive && walkingDustEffect)
    {
        updateWalkingDustPosition();
    }

    if (chargeAttackEffectActive && chargeAttackEffect)
    {
        updateChargeAttackEffectPosition();
    }

    updateBasicAttackEffectLifetime(Time::getDeltaTime());
}

void PaladinVFX::setWalkingDustActive(bool active)
{
    if (walkingDustActive == active)
    {
        return;
    }

    walkingDustActive = active;

    if (walkingDustActive)
    {
        addWalkingDust();
    }
    else
    {
        removeWalkingDust();
    }
}

void PaladinVFX::startChargeAttackEffect()
{
    if (chargeAttackEffectActive)
    {
        return;
    }

    chargeAttackEffectActive = true;
    addChargeAttackEffect();
}

void PaladinVFX::stopChargeAttackEffect()
{
    chargeAttackEffectActive = false;
    removeChargeAttackEffect();
}

void PaladinVFX::startBasicAttackTelegraph(
    const Vector3& position,
    const Vector3& rotation
)
{
    addBasicAttackTelegraph(position, rotation);
}

void PaladinVFX::stopBasicAttackTelegraph()
{
    removeBasicAttackTelegraph();
}

void PaladinVFX::playBasicAttackEffect()
{
    addBasicAttackEffect();

    if (basicAttackEffect)
    {
        basicAttackEffectTimer = basicAttackEffectLifetime;
    }
    else
    {
        basicAttackEffectTimer = 0.0f;
    }
}

void PaladinVFX::stopWalkingDust()
{
    walkingDustActive = false;
    removeWalkingDust();
}

Vector3 PaladinVFX::getWalkingDustPosition() const
{
    GameObject* owner = getOwner();
    Transform* ownerTransform = GameObjectAPI::getTransform(owner);

    if (!ownerTransform)
    {
        return Vector3(0.0f, 0.0f, 0.0f);
    }

    const Vector3 ownerPosition =
        TransformAPI::getGlobalPosition(ownerTransform);

    const Vector3 ownerForward =
        TransformAPI::getForward(ownerTransform);

    return Vector3(
        ownerPosition.x +
        ownerForward.x * walkingDustForwardOffset,
        ownerPosition.y + walkingDustYOffset,
        ownerPosition.z +
        ownerForward.z * walkingDustForwardOffset
    );
}

Vector3 PaladinVFX::getChargeAttackEffectPosition() const
{
    GameObject* owner = getOwner();
    Transform* ownerTransform = GameObjectAPI::getTransform(owner);

    if (!ownerTransform)
    {
        return Vector3(0.0f, 0.0f, 0.0f);
    }

    const Vector3 ownerPosition =
        TransformAPI::getGlobalPosition(ownerTransform);

    const Vector3 ownerForward =
        TransformAPI::getForward(ownerTransform);

    return Vector3(
        ownerPosition.x +
        ownerForward.x * chargeAttackForwardOffset,
        ownerPosition.y + chargeAttackYOffset,
        ownerPosition.z +
        ownerForward.z * chargeAttackForwardOffset
    );
}

Vector3 PaladinVFX::getBasicAttackEffectPosition() const
{
    GameObject* owner = getOwner();
    Transform* ownerTransform = GameObjectAPI::getTransform(owner);

    if (!ownerTransform)
    {
        return Vector3(0.0f, 0.0f, 0.0f);
    }

    const Vector3 ownerPosition =
        TransformAPI::getGlobalPosition(ownerTransform);

    const Vector3 ownerForward =
        TransformAPI::getForward(ownerTransform);

    return Vector3(
        ownerPosition.x +
        ownerForward.x * basicAttackForwardOffset,
        ownerPosition.y + basicAttackYOffset,
        ownerPosition.z +
        ownerForward.z * basicAttackForwardOffset
    );
}

Vector3 PaladinVFX::getOwnerRotation() const
{
    GameObject* owner = getOwner();
    Transform* ownerTransform = GameObjectAPI::getTransform(owner);

    if (!ownerTransform)
    {
        return Vector3(0.0f, 0.0f, 0.0f);
    }

    return TransformAPI::getGlobalEulerDegrees(ownerTransform);
}

void PaladinVFX::ensureWalkingDust()
{
    ParticleLifecycle::ensurePersistent(
        walkingDustEffect,
        m_walkingDustPrefab.m_id,
        getWalkingDustPosition(),
        getOwnerRotation()
    );
}

void PaladinVFX::ensureChargeAttackEffect()
{
    ParticleLifecycle::ensurePersistent(
        chargeAttackEffect,
        m_chargeAttackEffectPrefab.m_id,
        getChargeAttackEffectPosition(),
        getOwnerRotation()
    );
}

void PaladinVFX::ensureBasicAttackTelegraph(const Vector3& position, const Vector3& rotation)
{
    ParticleLifecycle::ensurePersistent(
        basicAttackTelegraph,
        m_basicAttackEffectPrefab.m_id,
        position,
        rotation
    );
}

void PaladinVFX::ensureBasicAttackEffect()
{
    ParticleLifecycle::ensurePersistent(
        basicAttackEffect,
        m_basicAttackEffectPrefab.m_id,
        getBasicAttackEffectPosition(),
        getOwnerRotation()
    );
}

void PaladinVFX::addWalkingDust()
{
    ensureWalkingDust();

    if (!walkingDustEffect)
    {
        Debug::warn(
            "[PaladinVFX] Could not instantiate WalkingDust prefab."
        );

        walkingDustActive = false;
        return;
    }

    updateWalkingDustPosition();
    ParticleLifecycle::activate(walkingDustEffect);
}

void PaladinVFX::removeWalkingDust()
{
    ParticleLifecycle::deactivate(walkingDustEffect);
}

void PaladinVFX::updateWalkingDustPosition()
{
    if (!walkingDustEffect)
    {
        return;
    }

    Transform* walkingDustTransform =
        GameObjectAPI::getTransform(walkingDustEffect);

    if (!walkingDustTransform)
    {
        return;
    }

    TransformAPI::setGlobalPosition(
        walkingDustTransform,
        getWalkingDustPosition()
    );

    TransformAPI::setGlobalRotationEuler(
        walkingDustTransform,
        getOwnerRotation()
    );
}

void PaladinVFX::addChargeAttackEffect()
{
    ensureChargeAttackEffect();

    if (!chargeAttackEffect)
    {
        Debug::warn(
            "[PaladinVFX] Could not instantiate ChargeAttackEffect prefab."
        );

        chargeAttackEffectActive = false;
        return;
    }

    updateChargeAttackEffectPosition();
    ParticleLifecycle::activate(chargeAttackEffect);
}

void PaladinVFX::removeChargeAttackEffect()
{
    ParticleLifecycle::deactivate(chargeAttackEffect);
}

void PaladinVFX::updateChargeAttackEffectPosition()
{
    if (!chargeAttackEffect)
    {
        return;
    }

    Transform* chargeAttackEffectTransform =
        GameObjectAPI::getTransform(chargeAttackEffect);

    if (!chargeAttackEffectTransform)
    {
        return;
    }

    TransformAPI::setGlobalPosition(
        chargeAttackEffectTransform,
        getChargeAttackEffectPosition()
    );

    TransformAPI::setGlobalRotationEuler(
        chargeAttackEffectTransform,
        getOwnerRotation()
    );
}

void PaladinVFX::addBasicAttackTelegraph(
    const Vector3& position,
    const Vector3& rotation
)
{
    Vector3 spawnPosition = position;
    spawnPosition.y += basicAttackTelegraphYOffset;

    ensureBasicAttackTelegraph(spawnPosition, rotation);

    if (!basicAttackTelegraph)
    {
        Debug::warn(
            "[PaladinVFX] Could not instantiate BasicAttackTelegraph."
        );
        return;
    }

    Transform* telegraphTransform = GameObjectAPI::getTransform(basicAttackTelegraph);
    if (telegraphTransform)
    {
        TransformAPI::setGlobalPosition(telegraphTransform, spawnPosition);
        TransformAPI::setGlobalRotationEuler(telegraphTransform, rotation);
    }

    ParticleLifecycle::activate(basicAttackTelegraph);
}

void PaladinVFX::removeBasicAttackTelegraph()
{
    ParticleLifecycle::deactivate(basicAttackTelegraph);
}

void PaladinVFX::addBasicAttackEffect()
{
    ensureBasicAttackEffect();

    if (!basicAttackEffect)
    {
        Debug::warn(
            "[PaladinVFX] Could not instantiate BasicAttackEffect prefab."
        );
        return;
    }

    Transform* effectTransform = GameObjectAPI::getTransform(basicAttackEffect);
    if (effectTransform)
    {
        TransformAPI::setGlobalPosition(effectTransform, getBasicAttackEffectPosition());
        TransformAPI::setGlobalRotationEuler(effectTransform, getOwnerRotation());
    }

    ParticleLifecycle::activate(basicAttackEffect);
}

void PaladinVFX::removeBasicAttackEffect()
{
    ParticleLifecycle::deactivate(basicAttackEffect);
}

void PaladinVFX::updateBasicAttackEffectLifetime(float deltaTime)
{
    if (!basicAttackEffect || basicAttackEffectTimer <= 0.0f)
    {
        return;
    }

    basicAttackEffectTimer -= deltaTime;

    if (basicAttackEffectTimer <= 0.0f)
    {
        removeBasicAttackEffect();
        basicAttackEffectTimer = 0.0f;
    }
}

IMPLEMENT_SCRIPT(PaladinVFX)

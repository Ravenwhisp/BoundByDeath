#include "pch.h"
#include "PaladinVFX.h"

namespace
{
    constexpr const char* WALKING_DUST_PREFAB_PATH =
        "Assets/Prefabs/Particles/WalkingDust.prefab";
}

IMPLEMENT_SCRIPT_FIELDS(PaladinVFX,
    SERIALIZED_FLOAT(walkingDustYOffset, "Walking Dust Y Offset", -5.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(walkingDustForwardOffset, "Walking Dust Forward Offset", -5.0f, 5.0f, 0.05f)
)

PaladinVFX::PaladinVFX(GameObject* owner)
    : Script(owner)
{
}

void PaladinVFX::Start()
{
    walkingDustEffect = nullptr;
    walkingDustActive = false;
}

void PaladinVFX::Update()
{
    if (walkingDustActive && walkingDustEffect)
    {
        updateWalkingDustPosition();
    }
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

    const Vector3 ownerPosition = TransformAPI::getGlobalPosition(ownerTransform);
    const Vector3 ownerForward = TransformAPI::getForward(ownerTransform);

    return Vector3(
        ownerPosition.x + ownerForward.x * walkingDustForwardOffset,
        ownerPosition.y + walkingDustYOffset,
        ownerPosition.z + ownerForward.z * walkingDustForwardOffset
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

void PaladinVFX::addWalkingDust()
{
    removeWalkingDust();

    walkingDustEffect = GameObjectAPI::instantiatePrefab(
        WALKING_DUST_PREFAB_PATH,
        getWalkingDustPosition(),
        getOwnerRotation()
    );

    if (!walkingDustEffect)
    {
        Debug::warn("[PaladinVFX] Could not instantiate WalkingDust prefab.");
        walkingDustActive = false;
    }
}

void PaladinVFX::removeWalkingDust()
{
    if (!walkingDustEffect)
    {
        return;
    }

    GameObjectAPI::removeGameObject(walkingDustEffect);
    walkingDustEffect = nullptr;
}

void PaladinVFX::updateWalkingDustPosition()
{
    Transform* walkingDustTransform = GameObjectAPI::getTransform(walkingDustEffect);

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

IMPLEMENT_SCRIPT(PaladinVFX)
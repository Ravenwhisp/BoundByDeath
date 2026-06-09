#include "pch.h"
#include "DeathTargetIndicatorUI.h"

#include "DeathConfig.h"

IMPLEMENT_SCRIPT_FIELDS_INHERITED(DeathTargetIndicatorUI, TargetIndicatorUI,
    FIELD_GROUP_LABEL("Range Indicator"),
    SERIALIZED_COMPONENT_REF(m_rangeIndicatorTransform, "Range Indicator Transform", ComponentType::TRANSFORM),
    SERIALIZED_FLOAT(m_heightOffset, "Height Offset", -1.0f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_rotationOffsetDegrees, "Rotation Offset Degrees", -180.0f, 180.0f, 1.0f),
    SERIALIZED_VEC3(m_rangeIndicatorFullScale, "Range Indicator Full Scale")
)

DeathTargetIndicatorUI::DeathTargetIndicatorUI(GameObject* owner)
    : TargetIndicatorUI(owner)
{
}

void DeathTargetIndicatorUI::onStart()
{
    Transform* playerTransform = m_playerTransform.getReferencedComponent();
    if (playerTransform == nullptr)
    {
        return;
    }

    GameObject* player = ComponentAPI::getOwner(playerTransform);
    if (player == nullptr)
    {
        return;
    }

    m_deathConfig = GameObjectAPI::findScript<DeathConfig>(player);

    if (m_deathConfig == nullptr)
    {
        Debug::warn("[DeathTargetIndicatorUI] DeathConfig not found on player '%s'.", GameObjectAPI::getName(player));
    }
}

void DeathTargetIndicatorUI::updateDirectionIndicator(GameObject* currentTarget)
{
    Transform* rangeTransform = m_rangeIndicatorTransform.getReferencedComponent();
    if (rangeTransform == nullptr)
    {
        hideDirectionIndicator();
        return;
    }

    if (m_deathConfig == nullptr)
    {
        hideDirectionIndicator();
        return;
    }

    Vector3 playerPosition;
    Vector3 direction;
    float targetDistance = 0.0f;

    if (!tryGetDirectionToTarget(currentTarget, playerPosition, direction))
    {
        hideDirectionIndicator();
        return;
    }

    GameObject* rangeObject = ComponentAPI::getOwner(rangeTransform);
    GameObjectAPI::setActive(rangeObject, true);

    updateRangeIndicatorTransform(rangeTransform, playerPosition, direction);
}

void DeathTargetIndicatorUI::hideDirectionIndicator()
{
    Transform* rangeTransform = m_rangeIndicatorTransform.getReferencedComponent();
    if (rangeTransform == nullptr)
    {
        return;
    }

    GameObject* rangeObject = ComponentAPI::getOwner(rangeTransform);
    GameObjectAPI::setActive(rangeObject, false);
}

bool DeathTargetIndicatorUI::tryGetDirectionToTarget(GameObject* currentTarget, Vector3& outPlayerPosition, Vector3& outDirection) const
{
    if (currentTarget == nullptr)
    {
        return false;
    }

    Transform* playerTransform = m_playerTransform.getReferencedComponent();
    Transform* targetTransform = GameObjectAPI::getTransform(currentTarget);

    if (playerTransform == nullptr || targetTransform == nullptr)
    {
        return false;
    }

    outPlayerPosition = TransformAPI::getGlobalPosition(playerTransform);
    const Vector3 targetPosition = TransformAPI::getGlobalPosition(targetTransform);

    Vector3 flatPlayerPosition = outPlayerPosition;
    Vector3 flatTargetPosition = targetPosition;

    flatPlayerPosition.y = 0.0f;
    flatTargetPosition.y = 0.0f;

    outDirection = flatTargetPosition - flatPlayerPosition;
    outDirection.y = 0.0f;

    const float distanceSq = outDirection.LengthSquared();
    if (distanceSq <= 0.0001f)
    {
        return false;
    }

    outDirection.Normalize();

    return true;
}

void DeathTargetIndicatorUI::updateRangeIndicatorTransform(Transform* rangeTransform, const Vector3& playerPosition, const Vector3& direction) const
{
    if (rangeTransform == nullptr || m_deathConfig == nullptr)
    {
        return;
    }

    Vector3 flatDirection = direction;
    flatDirection.y = 0.0f;

    if (flatDirection.LengthSquared() <= 0.0001f)
    {
        return;
    }

    flatDirection.Normalize();

    const float attackRange = m_deathConfig->m_basicAttackRange;
    const float visualLength = attackRange;

    Vector3 rangePosition = playerPosition + flatDirection * (visualLength * 0.5f);
    rangePosition.y = playerPosition.y + m_heightOffset;

    TransformAPI::setGlobalPosition(rangeTransform, rangePosition);

    const float angleRadians = atan2f(flatDirection.z, flatDirection.x);
    const float angleDegrees = DirectX::XMConvertToDegrees(angleRadians) + m_rotationOffsetDegrees;

    TransformAPI::setRotationEuler(rangeTransform, Vector3(0.0f, 0.0f, angleDegrees));

    TransformAPI::setScale(rangeTransform, m_rangeIndicatorFullScale);
}

IMPLEMENT_SCRIPT(DeathTargetIndicatorUI)
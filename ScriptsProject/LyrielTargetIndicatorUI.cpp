#include "pch.h"
#include "LyrielTargetIndicatorUI.h"

IMPLEMENT_SCRIPT_FIELDS_INHERITED(LyrielTargetIndicatorUI, TargetIndicatorUI,
    FIELD_GROUP_LABEL("Direction Arrow"),
    SERIALIZED_COMPONENT_REF(m_directionArrowTransform, "Direction Arrow Transform", ComponentType::TRANSFORM),
    SERIALIZED_FLOAT(m_forwardOffset, "Forward Offset", 0.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_heightOffset, "Height Offset", -1.0f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_rotationOffsetDegrees, "Rotation Offset Degrees", -180.0f, 180.0f, 1.0f),
    SERIALIZED_FLOAT(m_minDistanceToShowArrow, "Min Distance To Show Arrow", 0.0f, 10.0f, 0.05f)
)

LyrielTargetIndicatorUI::LyrielTargetIndicatorUI(GameObject* owner)
    : TargetIndicatorUI(owner)
{
}

void LyrielTargetIndicatorUI::updateDirectionIndicator(GameObject* currentTarget)
{
    Transform* arrowTransform = m_directionArrowTransform.getReferencedComponent();
    if (arrowTransform == nullptr)
    {
        hideDirectionIndicator();
        return;
    }

    Vector3 playerPosition;
    Vector3 direction;
    float distanceToTarget = 0.0f;

    if (!tryGetDirectionToTarget(currentTarget, playerPosition, direction, distanceToTarget))
    {
        hideDirectionIndicator();
        return;
    }

    if (distanceToTarget <= m_minDistanceToShowArrow)
    {
        hideDirectionIndicator();
        return;
    }

    GameObject* arrowObject = ComponentAPI::getOwner(arrowTransform);
    GameObjectAPI::setActive(arrowObject, true);

    updateDirectionArrowTransform(arrowTransform, playerPosition, direction);
}

void LyrielTargetIndicatorUI::hideDirectionIndicator()
{
    Transform* arrowTransform = m_directionArrowTransform.getReferencedComponent();
    if (arrowTransform == nullptr)
    {
        return;
    }

    GameObject* arrowObject = ComponentAPI::getOwner(arrowTransform);
    GameObjectAPI::setActive(arrowObject, false);
}

bool LyrielTargetIndicatorUI::tryGetDirectionToTarget(GameObject* currentTarget, Vector3& outPlayerPosition, Vector3& outDirection, float& outDistance) const
{
    outDistance = 0.0f;

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

    outDistance = sqrtf(distanceSq);
    outDirection.Normalize();

    return true;
}

void LyrielTargetIndicatorUI::updateDirectionArrowTransform(Transform* arrowTransform, const Vector3& playerPosition, const Vector3& direction) const
{
    if (arrowTransform == nullptr)
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

    Vector3 arrowPosition = playerPosition + flatDirection * m_forwardOffset;
    arrowPosition.y = playerPosition.y + m_heightOffset;

    TransformAPI::setGlobalPosition(arrowTransform, arrowPosition);

    const float angleRadians = atan2f(flatDirection.z, flatDirection.x);
    const float angleDegrees = DirectX::XMConvertToDegrees(angleRadians) + m_rotationOffsetDegrees;

    TransformAPI::setRotationEuler(arrowTransform, Vector3(0.0f, 0.0f, angleDegrees));
}

IMPLEMENT_SCRIPT(LyrielTargetIndicatorUI)
#include "pch.h"
#include "LyrielTargetIndicatorUI.h"

IMPLEMENT_SCRIPT_FIELDS_INHERITED(LyrielTargetIndicatorUI, TargetIndicatorUI,
    FIELD_GROUP_LABEL("Direction Arrow"),
    SERIALIZED_COMPONENT_REF(m_directionArrowTransform, "Direction Arrow Transform", ComponentType::TRANSFORM),
    SERIALIZED_FLOAT(m_floorYOffset, "Floor Y Offset", -1.0f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_forwardOffset, "Forward Offset", 0.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_rotationOffsetDegrees, "Rotation Offset Degrees", -180.0f, 180.0f, 1.0f)
)

LyrielTargetIndicatorUI::LyrielTargetIndicatorUI(GameObject* owner)
    : TargetIndicatorUI(owner)
{
}

void LyrielTargetIndicatorUI::onStart()
{
    Transform* arrowTransform = m_directionArrowTransform.getReferencedComponent();
    if (arrowTransform != nullptr)
    {
        m_directionArrowBaseScale = TransformAPI::getScale(arrowTransform);
    }
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

    if (!tryGetDirectionToTarget(currentTarget, playerPosition, direction))
    {
        hideDirectionIndicator();
        return;
    }

    GameObject* arrowObject = ComponentAPI::getOwner(arrowTransform);
    GameObjectAPI::setActive(arrowObject, true);

    Vector3 arrowPosition = playerPosition + direction * m_forwardOffset;
    arrowPosition.y = playerPosition.y + m_floorYOffset;

    updateDirectionArrowTransform(arrowTransform, arrowPosition, direction);
    TransformAPI::setScale(arrowTransform, m_directionArrowBaseScale);
}

void LyrielTargetIndicatorUI::hideDirectionIndicator()
{
    Transform* arrowTransform = m_directionArrowTransform.getReferencedComponent();
    if (arrowTransform == nullptr)
    {
        return;
    }

    TransformAPI::setScale(arrowTransform, m_directionArrowBaseScale);

    GameObject* arrowObject = ComponentAPI::getOwner(arrowTransform);
    GameObjectAPI::setActive(arrowObject, false);
}

bool LyrielTargetIndicatorUI::tryGetDirectionToTarget(GameObject* currentTarget, Vector3& outPlayerPosition, Vector3& outDirection) const
{
    if (currentTarget == nullptr)
    {
        return false;
    }

    Transform* playerTransform = m_playerTransform.getReferencedComponent();
    Transform* targetTransform = GameObjectAPI::getTransform(currentTarget);

    if (playerTransform == nullptr)
    {
        return false;
    }

    outPlayerPosition = TransformAPI::getGlobalPosition(playerTransform);
    Vector3 targetPosition = TransformAPI::getGlobalPosition(targetTransform);

    outPlayerPosition.y = 0.0f;
    targetPosition.y = 0.0f;

    outDirection = targetPosition - outPlayerPosition;
    outDirection.y = 0.0f;

    if (outDirection.LengthSquared() <= 0.0001f)
    {
        return false;
    }

    outDirection.Normalize();
    return true;
}

void LyrielTargetIndicatorUI::updateDirectionArrowTransform(Transform* arrowTransform, const Vector3& position, const Vector3& direction) const
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

    TransformAPI::setGlobalPosition(arrowTransform, position);

    const float yawRadians = atan2f(flatDirection.x, flatDirection.z);
    const float yawDegrees = DirectX::XMConvertToDegrees(yawRadians) + m_rotationOffsetDegrees;

    TransformAPI::setGlobalRotationEuler(arrowTransform, Vector3(0.0f, yawDegrees, 0.0f));
}

IMPLEMENT_SCRIPT(LyrielTargetIndicatorUI)
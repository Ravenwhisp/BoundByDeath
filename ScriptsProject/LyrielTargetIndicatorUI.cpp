#include "pch.h"
#include "LyrielTargetIndicatorUI.h"

IMPLEMENT_SCRIPT_FIELDS_INHERITED(LyrielTargetIndicatorUI, TargetIndicatorUI,
    FIELD_GROUP_LABEL("Direction Arrow"),
    SERIALIZED_COMPONENT_REF(m_directionArrowTransform, "Direction Arrow Transform", ComponentType::TRANSFORM),
    SERIALIZED_COMPONENT_REF(m_directionOriginTransform, "Direction Origin Transform", ComponentType::TRANSFORM),
    SERIALIZED_FLOAT(m_forwardOffset, "Forward Offset", 0.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_rotationOffsetDegrees, "Rotation Offset Degrees", -180.0f, 180.0f, 1.0f)
)

LyrielTargetIndicatorUI::LyrielTargetIndicatorUI(GameObject* owner)
    : TargetIndicatorUI(owner)
{
}

void LyrielTargetIndicatorUI::updateDirectionIndicator(GameObject* currentTarget)
{
    if (currentTarget == nullptr)
    {
        hideDirectionIndicator();
        return;
    }

    Transform* arrowTransform = m_directionArrowTransform.getReferencedComponent();
    if (arrowTransform == nullptr)
    {
        hideDirectionIndicator();
        return;
    }

    Vector3 originPosition;
    Vector3 direction;

    if (!tryGetDirectionToTarget(originPosition, direction))
    {
        hideDirectionIndicator();
        return;
    }

    GameObject* arrowObject = ComponentAPI::getOwner(arrowTransform);
    GameObjectAPI::setActive(arrowObject, true);

    updateDirectionArrowTransform(arrowTransform, originPosition, direction);
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

bool LyrielTargetIndicatorUI::tryGetDirectionToTarget(Vector3& outOriginPosition, Vector3& outDirection) const
{
    Transform* originTransform = m_directionOriginTransform.getReferencedComponent();
    Transform* targetIndicatorTransform = m_targetIndicatorTransform.getReferencedComponent();

    if (originTransform == nullptr || targetIndicatorTransform == nullptr)
    {
        return false;
    }

    outOriginPosition = TransformAPI::getGlobalPosition(originTransform);
    const Vector3 targetIndicatorPosition = TransformAPI::getGlobalPosition(targetIndicatorTransform);

    Vector3 flatOriginPosition = outOriginPosition;
    Vector3 flatTargetPosition = targetIndicatorPosition;

    flatOriginPosition.z = 0.0f;
    flatTargetPosition.z = 0.0f;

    outDirection = flatTargetPosition - flatOriginPosition;
    outDirection.z = 0.0f;

    if (outDirection.LengthSquared() <= 0.0001f)
    {
        return false;
    }

    outDirection.Normalize();
    return true;
}

void LyrielTargetIndicatorUI::updateDirectionArrowTransform(Transform* arrowTransform, const Vector3& originPosition, const Vector3& direction) const
{
    if (arrowTransform == nullptr)
    {
        return;
    }

    Vector3 flatDirection = direction;
    flatDirection.z = 0.0f;

    if (flatDirection.LengthSquared() <= 0.0001f)
    {
        return;
    }

    flatDirection.Normalize();

    Vector3 arrowPosition = originPosition + flatDirection * m_forwardOffset;

    arrowPosition.z = originPosition.z;

    TransformAPI::setGlobalPosition(arrowTransform, arrowPosition);

    const float angleRadians = atan2f(flatDirection.y, flatDirection.x);
    const float angleDegrees = DirectX::XMConvertToDegrees(angleRadians) - 90.0f + m_rotationOffsetDegrees;

    TransformAPI::setGlobalRotationEuler(arrowTransform, Vector3(0.0f, 0.0f, angleDegrees));
}

IMPLEMENT_SCRIPT(LyrielTargetIndicatorUI)
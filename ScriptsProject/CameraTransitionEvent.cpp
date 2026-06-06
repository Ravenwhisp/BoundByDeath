#include "pch.h"
#include "CameraTransitionEvent.h"

#include "GameplayEventTrigger.h"
#include "CameraTransitionController.h"

IMPLEMENT_SCRIPT_FIELDS(CameraTransitionEvent,
    SERIALIZED_FLOAT(m_moveToDuration, "Move To Duration", 0.0f, 20.0f, 0.05f),
    SERIALIZED_FLOAT(m_holdDuration, "Hold Duration", 0.0f, 20.0f, 0.05f),
    SERIALIZED_FLOAT(m_returnDuration, "Return Duration", 0.0f, 20.0f, 0.05f)
)

CameraTransitionEvent::CameraTransitionEvent(GameObject* owner)
    : GameplayEventAction(owner)
{
}

void CameraTransitionEvent::Start()
{
    m_targetPoint = findTargetPoint();

    if (m_targetPoint == nullptr)
    {
        Debug::warn("CameraTransitionEvent on '%s' could not find child path 'CameraPoints/Point1'.", GameObjectAPI::getName(getOwner()));
    }
}

void CameraTransitionEvent::executeEvent(GameplayEventTrigger* trigger)
{
    if (m_targetPoint == nullptr)
    {
        return;
    }

    CameraTransitionController* cameraTransitionController = findCameraTransitionController();
    if (cameraTransitionController == nullptr)
    {
        Debug::warn("CameraTransitionEvent on '%s' could not find CameraTransitionController on the default camera.",GameObjectAPI::getName(getOwner()));
        return;
    }

    cameraTransitionController->startTransition(this);
}

Transform* CameraTransitionEvent::findTargetPoint() const
{
    GameObject* owner = getOwner();
    if (owner == nullptr)
    {
        return nullptr;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(owner);
    if (ownerTransform == nullptr)
    {
        return nullptr;
    }

    Transform* cameraPointsRoot = TransformAPI::findChildByName(ownerTransform, "CameraPoints");
    if (cameraPointsRoot == nullptr)
    {
        return nullptr;
    }

    return TransformAPI::findChildByName(cameraPointsRoot, "Point1");
}

CameraTransitionController* CameraTransitionEvent::findCameraTransitionController() const
{
    GameObject* defaultCamera = SceneAPI::getDefaultCameraGameObject();
    if (defaultCamera == nullptr)
    {
        return nullptr;
    }

    return GameObjectAPI::findScript<CameraTransitionController>(defaultCamera);
}

IMPLEMENT_SCRIPT(CameraTransitionEvent)
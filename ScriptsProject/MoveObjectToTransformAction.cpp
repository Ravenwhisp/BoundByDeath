#include "pch.h"
#include "MoveObjectToTransformAction.h"

#include "CameraTransitionController.h"
#include "CameraTransitionStep.h"

static const char* cameraStepActionTriggerNames[] =
{
    "Step Started",
    "Step Reached",
    "Step Finished"
};

constexpr int cameraStepActionTriggerCount = 3;

IMPLEMENT_SCRIPT_FIELDS(MoveObjectToTransformAction,
    SERIALIZED_COMPONENT_REF(m_objectToMove, "Object To Move", ComponentType::TRANSFORM),
    SERIALIZED_COMPONENT_REF(m_targetTransform, "Target Transform", ComponentType::TRANSFORM),

    SERIALIZED_ENUM_INT(m_triggerMoment, "Trigger Moment", cameraStepActionTriggerNames, cameraStepActionTriggerCount),

    SERIALIZED_FLOAT(m_moveDuration, "Move Duration", 0.0f, 20.0f, 0.05f),
)

MoveObjectToTransformAction::MoveObjectToTransformAction(GameObject* owner)
    : CameraTransitionStepAction(owner)
{
}

void MoveObjectToTransformAction::Update()
{
    if (!m_isMoving)
    {
        return;
    }

    const float dt = Time::getDeltaTime();
    updateMove(dt);
}

void MoveObjectToTransformAction::onStepStarted(CameraTransitionController* controller, CameraTransitionStep* step)
{
    if (!shouldRunOnStepStarted(m_triggerMoment))
    {
        return;
    }

    tryStartMove(controller, step);
}

void MoveObjectToTransformAction::onStepReached(CameraTransitionController* controller, CameraTransitionStep* step)
{
    if (!shouldRunOnStepReached(m_triggerMoment))
    {
        return;
    }

    tryStartMove(controller, step);
}

void MoveObjectToTransformAction::onStepFinished(CameraTransitionController* controller, CameraTransitionStep* step)
{
    if (!shouldRunOnStepFinished(m_triggerMoment))
    {
        return;
    }

    tryStartMove(controller, step);
}

void MoveObjectToTransformAction::tryStartMove(CameraTransitionController* controller, CameraTransitionStep* step)
{
    Transform* objectToMove = m_objectToMove.getReferencedComponent();
    Transform* targetTransform = m_targetTransform.getReferencedComponent();

    if (objectToMove == nullptr)
    {
        Debug::warn("MoveObjectToTransformAction on '%s' has no valid Object To Move assigned.", GameObjectAPI::getName(getOwner()));
        return;
    }

    if (targetTransform == nullptr)
    {
        Debug::warn("MoveObjectToTransformAction on '%s' has no valid Target Transform assigned.", GameObjectAPI::getName(getOwner()));
        return;
    }

    startMove();
}

void MoveObjectToTransformAction::startMove()
{
    Transform* objectToMove = m_objectToMove.getReferencedComponent();
    Transform* targetTransform = m_targetTransform.getReferencedComponent();

    if (objectToMove == nullptr || targetTransform == nullptr)
    {
        return;
    }

    m_isMoving = true;
    m_timer = 0.0f;

    m_startPosition = TransformAPI::getGlobalPosition(objectToMove);
    m_startRotation = TransformAPI::getGlobalEulerDegrees(objectToMove);

    m_targetPosition = TransformAPI::getGlobalPosition(targetTransform);
    m_targetRotation = TransformAPI::getGlobalEulerDegrees(targetTransform);

    if (m_moveDuration <= 0.0001f)
    {
        finishMove();
    }
}

void MoveObjectToTransformAction::updateMove(float dt)
{
    Transform* objectToMove = m_objectToMove.getReferencedComponent();
    if (objectToMove == nullptr)
    {
        m_isMoving = false;
        return;
    }

    m_timer += dt;

    float alpha = m_timer / m_moveDuration;
    if (alpha > 1.0f)
    {
        alpha = 1.0f;
    }

    alpha = MathAPI::smoothStep(0.0f, 1.0f, alpha);

    const Vector3 newPosition = MathAPI::lerp(m_startPosition, m_targetPosition, alpha);
    const Vector3 newRotation = MathAPI::lerp(m_startRotation, m_targetRotation, alpha);

    TransformAPI::setGlobalPosition(objectToMove, newPosition);
    TransformAPI::setGlobalRotationEuler(objectToMove, newRotation);

    if (m_timer >= m_moveDuration)
    {
        finishMove();
    }
}

void MoveObjectToTransformAction::finishMove()
{
    Transform* objectToMove = m_objectToMove.getReferencedComponent();
    if (objectToMove == nullptr)
    {
        m_isMoving = false;
        return;
    }

    TransformAPI::setGlobalPosition(objectToMove, m_targetPosition);
    TransformAPI::setGlobalRotationEuler(objectToMove, m_targetRotation);

    m_isMoving = false;
    m_timer = 0.0f;
}

IMPLEMENT_SCRIPT(MoveObjectToTransformAction)
#include "pch.h"
#include "MoveObjectToTransformAction.h"

#include "CameraTransitionController.h"
#include "CameraTransitionStep.h"

IMPLEMENT_SCRIPT_FIELDS_INHERITED(MoveObjectToTransformAction, CameraTransitionStepAction,
    SERIALIZED_COMPONENT_REF(m_objectToMove, "Object To Move", ComponentType::TRANSFORM),
    SERIALIZED_COMPONENT_REF(m_targetTransform, "Target Transform", ComponentType::TRANSFORM),
    SERIALIZED_FLOAT(m_moveDuration, "Move Duration", 0.0f, 20.0f, 0.05f),

    FIELD_GROUP_COLLAPSE("Animations",
        SERIALIZED_BOOL(m_playAnimationWhileMoving, "Play Animation While Moving"),
        SERIALIZED_STRING(m_movingStateName, "Moving State Name"),
        SERIALIZED_BOOL(m_playAnimationOnFinish, "Play Animation On Finish"),
        SERIALIZED_STRING(m_finishStateName, "Finish State Name")
    )
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
    if (!shouldRunOnStepStarted())
    {
        return;
    }

    tryStartMove(controller, step);
}

void MoveObjectToTransformAction::onStepReached(CameraTransitionController* controller, CameraTransitionStep* step)
{
    if (!shouldRunOnStepReached())
    {
        return;
    }

    tryStartMove(controller, step);
}

void MoveObjectToTransformAction::onStepFinished(CameraTransitionController* controller, CameraTransitionStep* step)
{
    if (!shouldRunOnStepFinished())
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

    if (m_playAnimationWhileMoving)
    {
        playAnimationState(m_movingStateName, 0.15);
    }

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

    if (m_playAnimationOnFinish)
    {
        playAnimationState(m_finishStateName, 0.15);
    }

    m_isMoving = false;
    m_timer = 0.0f;
}

void MoveObjectToTransformAction::playAnimationState(const std::string& stateName, float transitionTimeSeconds)
{
    if (stateName.empty())
    {
        return;
    }

    Transform* objectToMove = m_objectToMove.getReferencedComponent();
    if (objectToMove == nullptr)
    {
        return;
    }

    GameObject* object = ComponentAPI::getOwner(objectToMove);

    AnimationComponent* animation = AnimationAPI::getAnimationComponent(object);
    if (animation == nullptr)
    {
        Debug::warn("MoveObjectToTransformAction on '%s' could not find AnimationComponent on moved object.", GameObjectAPI::getName(getOwner()));
        return;
    }

    const bool success = AnimationAPI::playState(animation, stateName.c_str(), transitionTimeSeconds);

    if (!success)
    {
        Debug::warn("MoveObjectToTransformAction on '%s' failed to play animation state '%s'.", GameObjectAPI::getName(getOwner()), stateName.c_str());
    }
}

IMPLEMENT_SCRIPT(MoveObjectToTransformAction)
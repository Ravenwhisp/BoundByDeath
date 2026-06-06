#include "pch.h"
#include "CameraTransitionController.h"

#include "PlayerController.h"
#include "CameraFollow.h"
#include "CameraTransitionEvent.h"

CameraTransitionController::CameraTransitionController(GameObject* owner)
    : Script(owner)
{
}

void CameraTransitionController::Start()
{
    m_cameraFollow = GameObjectAPI::findScript<CameraFollow>(getOwner());

    if (m_cameraFollow == nullptr)
    {
        Debug::warn("CameraTransitionController on '%s' could not find CameraFollow on the same GameObject.", GameObjectAPI::getName(getOwner()));
    }

    findPlayerControllers();
}

void CameraTransitionController::Update()
{
    if (!m_isTransitioning)
    {
        return;
    }

    const float dt = Time::getDeltaTime();

    switch (m_state)
    {
    case TransitionState::MovingToTarget:
        updateMovingToTarget(dt);
        break;

    case TransitionState::Holding:
        updateHolding(dt);
        break;

    case TransitionState::Returning:
        updateReturning(dt);
        break;

    case TransitionState::None:
    default:
        break;
    }
}

void CameraTransitionController::startTransition(CameraTransitionEvent* event)
{
    if (m_isTransitioning)
    {
        return;
    }

    startMovingToTarget(event);
}

void CameraTransitionController::startMovingToTarget(CameraTransitionEvent* event)
{
    Transform* cameraTransform = GameObjectAPI::getTransform(getOwner());
    Transform* targetPoint = event->getTargetPoint();
    if (targetPoint == nullptr)
    {
        Debug::warn("CameraTransitionController on '%s' could not start transition because target point is null.", GameObjectAPI::getName(getOwner()));
        return;
    }

    m_currentEvent = event;
    m_isTransitioning = true;
    m_state = TransitionState::MovingToTarget;
    m_timer = 0.0f;

    m_startPosition = TransformAPI::getGlobalPosition(cameraTransform);
    m_startRotation = TransformAPI::getGlobalEulerDegrees(cameraTransform);

    m_targetPosition = TransformAPI::getGlobalPosition(targetPoint);
    m_targetRotation = TransformAPI::getGlobalEulerDegrees(targetPoint);

    if (m_cameraFollow != nullptr)
    {
        m_cameraFollow->setFollowEnabled(false);
    }

    setPlayersGameplayInputLocked(true);
}

void CameraTransitionController::updateMovingToTarget(float dt)
{
    Transform* cameraTransform = GameObjectAPI::getTransform(getOwner());

    const float duration = m_currentEvent->getMoveToDuration();

    m_timer += dt;

    const float normalizedTime = m_timer / duration;
    const float alpha = MathAPI::smoothStep(0.0f, 1.0f, normalizedTime);

    const Vector3 newPosition = MathAPI::lerp(m_startPosition, m_targetPosition, alpha);
    const Vector3 newRotation = MathAPI::lerp(m_startRotation, m_targetRotation, alpha);

    TransformAPI::setGlobalPosition(cameraTransform, newPosition);
    TransformAPI::setGlobalRotationEuler(cameraTransform, newRotation);

    if (m_timer >= duration)
    {
        TransformAPI::setGlobalPosition(cameraTransform, m_targetPosition);
        TransformAPI::setGlobalRotationEuler(cameraTransform, m_targetRotation);

        m_state = TransitionState::Holding;
        m_timer = 0.0f;
    }
}

void CameraTransitionController::updateHolding(float dt)
{
    const float duration = m_currentEvent->getHoldDuration();

    m_timer += dt;

    if (m_timer < duration)
    {
        return;
    }

    Transform* cameraTransform = GameObjectAPI::getTransform(getOwner());

    m_returnStartPosition = TransformAPI::getGlobalPosition(cameraTransform);
    m_returnStartRotation = TransformAPI::getGlobalEulerDegrees(cameraTransform);

    m_state = TransitionState::Returning;
    m_timer = 0.0f;
}

void CameraTransitionController::updateReturning(float dt)
{
    Transform* cameraTransform = GameObjectAPI::getTransform(getOwner());

    const float duration = m_currentEvent->getReturnDuration();

    m_timer += dt;

    const float normalizedTime = m_timer / duration;
    const float alpha = MathAPI::smoothStep(0.0f, 1.0f, normalizedTime);

    const Vector3 newPosition = MathAPI::lerp(m_returnStartPosition, m_startPosition, alpha);
    const Vector3 newRotation = MathAPI::lerp(m_returnStartRotation, m_startRotation, alpha);

    TransformAPI::setGlobalPosition(cameraTransform, newPosition);
    TransformAPI::setGlobalRotationEuler(cameraTransform, newRotation);

    if (m_timer >= duration)
    {
        TransformAPI::setGlobalPosition(cameraTransform, m_startPosition);
        TransformAPI::setGlobalRotationEuler(cameraTransform, m_startRotation);

        finishTransition();
    }
}

void CameraTransitionController::finishTransition()
{
    if (m_cameraFollow != nullptr)
    {
        m_cameraFollow->setFollowEnabled(true);
    }

    setPlayersGameplayInputLocked(false);

    m_currentEvent = nullptr;
    m_state = TransitionState::None;
    m_isTransitioning = false;
    m_timer = 0.0f;
}

void CameraTransitionController::findPlayerControllers()
{
    m_playerControllers.clear();

    const std::vector<GameObject*> players = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER, true);

    for (GameObject* player : players)
    {
        PlayerController* playerController = GameObjectAPI::findScript<PlayerController>(player);
        if (playerController == nullptr)
        {
            Debug::warn("CameraTransitionController could not find PlayerController on player '%s'.", GameObjectAPI::getName(player));
            continue;
        }

        m_playerControllers.push_back(playerController);
    }
}

void CameraTransitionController::setPlayersGameplayInputLocked(bool locked)
{
    for (PlayerController* playerController : m_playerControllers)
    {
        if (playerController == nullptr)
        {
            continue;
        }

        playerController->setGameplayInputLocked(locked);
    }
}

IMPLEMENT_SCRIPT(CameraTransitionController)
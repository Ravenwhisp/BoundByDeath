#include "pch.h"
#include "CameraTransitionController.h"

#include "PlayerController.h"
#include "CameraFollow.h"
#include "CameraTransitionEvent.h"
#include "Damageable.h"

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

    m_currentPointIndex = 0;
    Transform* targetPoint = event->getTargetPoint(m_currentPointIndex);

    if (targetPoint == nullptr)
    {
        Debug::warn("CameraTransitionController on '%s' could not start transition because target point is null.", GameObjectAPI::getName(getOwner()));
        return;
    }

    m_currentEvent = event;
    m_isTransitioning = true;
    m_state = TransitionState::MovingToTarget;
    m_timer = 0.0f;

    m_transitionStartPosition = TransformAPI::getGlobalPosition(cameraTransform);
    m_transitionStartRotation = TransformAPI::getGlobalEulerDegrees(cameraTransform);

    m_segmentStartPosition = m_transitionStartPosition;
    m_segmentStartRotation = m_transitionStartRotation;

    m_targetPosition = TransformAPI::getGlobalPosition(targetPoint);
    m_targetRotation = TransformAPI::getGlobalEulerDegrees(targetPoint);

    if (m_cameraFollow != nullptr)
    {
        m_cameraFollow->setFollowEnabled(false);
    }

    setPlayersGameplayInputLocked(true);
    setPlayersInvulnerable(true);
}

void CameraTransitionController::updateMovingToTarget(float dt)
{
    Transform* cameraTransform = GameObjectAPI::getTransform(getOwner());

    const float duration = m_currentEvent->getMoveToDuration();

    m_timer += dt;

    const float normalizedTime = m_timer / duration;
    const float alpha = normalizedTime;

    const Vector3 newPosition = MathAPI::lerp(m_segmentStartPosition, m_targetPosition, alpha);
    const Vector3 newRotation = MathAPI::lerp(m_segmentStartRotation, m_targetRotation, alpha);

    TransformAPI::setGlobalPosition(cameraTransform, newPosition);
    TransformAPI::setGlobalRotationEuler(cameraTransform, newRotation);

    if (m_timer >= duration)
    {
        TransformAPI::setGlobalPosition(cameraTransform, m_targetPosition);
        TransformAPI::setGlobalRotationEuler(cameraTransform, m_targetRotation);

        m_currentPointIndex++;

        if (m_currentPointIndex < m_currentEvent->getTargetPointCount())
        {
            Transform* nextPoint = m_currentEvent->getTargetPoint(m_currentPointIndex);

            m_segmentStartPosition = m_targetPosition;
            m_segmentStartRotation = m_targetRotation;

            m_targetPosition = TransformAPI::getGlobalPosition(nextPoint);
            m_targetRotation = TransformAPI::getGlobalEulerDegrees(nextPoint);

            m_timer = 0.0f;
            return;
        }

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

    const Vector3 newPosition = MathAPI::lerp(m_returnStartPosition, m_transitionStartPosition, alpha);
    const Vector3 newRotation = MathAPI::lerp(m_returnStartRotation, m_transitionStartRotation, alpha);

    TransformAPI::setGlobalPosition(cameraTransform, newPosition);
    TransformAPI::setGlobalRotationEuler(cameraTransform, newRotation);

    if (m_timer >= duration)
    {
        TransformAPI::setGlobalPosition(cameraTransform, m_transitionStartPosition);
        TransformAPI::setGlobalRotationEuler(cameraTransform, m_transitionStartRotation);

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
    setPlayersInvulnerable(false);

    m_currentEvent = nullptr;
    m_state = TransitionState::None;
    m_isTransitioning = false;
    m_timer = 0.0f;
}

void CameraTransitionController::findPlayerControllers()
{
    m_playerControllers.clear();
    m_playerDamageables.clear();

    const std::vector<GameObject*> players = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER, true);

    for (GameObject* player : players)
    {
        PlayerController* playerController = GameObjectAPI::findScript<PlayerController>(player);
        if (playerController == nullptr)
        {
            Debug::warn("CameraTransitionController could not find PlayerController on player '%s'.", GameObjectAPI::getName(player));
        }
        else
        {
            m_playerControllers.push_back(playerController);
        }

        Damageable* damageable = GameObjectAPI::findScript<Damageable>(player);
        if (damageable == nullptr)
        {
            Debug::warn("CameraTransitionController could not find Damageable on player '%s'.", GameObjectAPI::getName(player));
        }
        else
        {
            m_playerDamageables.push_back(damageable);
        }
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

void CameraTransitionController::setPlayersInvulnerable(bool invulnerable)
{
    for (Damageable* damageable : m_playerDamageables)
    {
        if (damageable == nullptr)
        {
            continue;
        }

        damageable->setInvulnerable(invulnerable);
    }
}

IMPLEMENT_SCRIPT(CameraTransitionController)
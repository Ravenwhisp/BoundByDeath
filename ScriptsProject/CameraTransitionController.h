#pragma once

#include "ScriptAPI.h"

class CameraFollow;
class CameraTransitionEvent;
class PlayerController;
class Damageable;

class CameraTransitionController : public Script
{
    DECLARE_SCRIPT(CameraTransitionController)

public:
    explicit CameraTransitionController(GameObject* owner);

    void Start() override;
    void Update() override;

    void startTransition(CameraTransitionEvent* event);

    bool isTransitioning() const { return m_isTransitioning; }

private:
    enum class TransitionState
    {
        None,
        MovingToTarget,
        Holding,
        Returning
    };

private:
    void startMovingToTarget(CameraTransitionEvent* event);
    void updateMovingToTarget(float dt);
    void updateHolding(float dt);
    void updateReturning(float dt);
    void finishTransition();

    void findPlayerControllers();
    void setPlayersGameplayInputLocked(bool locked);
    void setPlayersInvulnerable(bool invulnerable);

private:
    CameraFollow* m_cameraFollow = nullptr;
    CameraTransitionEvent* m_currentEvent = nullptr;
    std::vector<PlayerController*> m_playerControllers;
    std::vector<Damageable*> m_playerDamageables;

    TransitionState m_state = TransitionState::None;

    bool m_isTransitioning = false;

    float m_timer = 0.0f;

    Vector3 m_transitionStartPosition = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 m_transitionStartRotation = Vector3(0.0f, 0.0f, 0.0f);

    Vector3 m_segmentStartPosition = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 m_segmentStartRotation = Vector3(0.0f, 0.0f, 0.0f);

    Vector3 m_targetPosition = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 m_targetRotation = Vector3(0.0f, 0.0f, 0.0f);
    int m_currentPointIndex = 0;

    Vector3 m_returnStartPosition = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 m_returnStartRotation = Vector3(0.0f, 0.0f, 0.0f);

};
#pragma once

#include "ScriptAPI.h"
#include "CameraTransitionStepAction.h"

class Transform;

class MoveObjectToTransformAction : public CameraTransitionStepAction
{
    DECLARE_SCRIPT(MoveObjectToTransformAction)

public:
    explicit MoveObjectToTransformAction(GameObject* owner);

    void Update() override;

    ScriptFieldList getExposedFields() const override;

    void onStepStarted(CameraTransitionController* controller, CameraTransitionStep* step) override;
    void onStepReached(CameraTransitionController* controller, CameraTransitionStep* step) override;
    void onStepFinished(CameraTransitionController* controller, CameraTransitionStep* step) override;

private:
    void tryStartMove(CameraTransitionController* controller, CameraTransitionStep* step);
    void startMove();
    void updateMove(float dt);
    void finishMove();

    void playAnimationState(const std::string& stateName, float transitionTimeSeconds);

public:
    ScriptComponentRef<Transform> m_objectToMove;
    ScriptComponentRef<Transform> m_targetTransform;

    float m_moveDuration = 1.0f;

    bool m_playAnimationWhileMoving = false;
    std::string m_movingStateName = "Walk";
    bool m_playAnimationOnFinish = false;
    std::string m_finishStateName = "Idle";

private:
    bool m_isMoving = false;
    float m_timer = 0.0f;

    Vector3 m_startPosition = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 m_startRotation = Vector3(0.0f, 0.0f, 0.0f);

    Vector3 m_targetPosition = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 m_targetRotation = Vector3(0.0f, 0.0f, 0.0f);
};
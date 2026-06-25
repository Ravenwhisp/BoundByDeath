#pragma once

#include "ScriptAPI.h"
#include "CameraTransitionStepAction.h"

class Transform;
class AnimationComponent;

class PlayAnimationAction : public CameraTransitionStepAction
{
    DECLARE_SCRIPT(PlayAnimationAction)

public:
    explicit PlayAnimationAction(GameObject* owner);

    ScriptFieldList getExposedFields() const override;

    void onStepStarted(CameraTransitionController* controller, CameraTransitionStep* step) override;
    void onStepReached(CameraTransitionController* controller, CameraTransitionStep* step) override;
    void onStepFinished(CameraTransitionController* controller, CameraTransitionStep* step) override;

private:
    void tryPlayAnimation();
    AnimationComponent* findAnimationComponent() const;

public:
    ScriptComponentRef<Transform> m_animationTarget;

    std::string m_stateName = "";
};
#pragma once

#include "ScriptAPI.h"

class CameraTransitionController;
class CameraTransitionStep;

enum class CameraTransitionStepActionTrigger
{
    StepStarted = 0,
    StepReached,
    StepFinished
};

class CameraTransitionStepAction : public Script
{
public:
    explicit CameraTransitionStepAction(GameObject* owner)
        : Script(owner)
    {
    }

    virtual void onStepStarted(CameraTransitionController* controller, CameraTransitionStep* step) {}

    virtual void onStepReached(CameraTransitionController* controller, CameraTransitionStep* step) {}

    virtual void onStepFinished(CameraTransitionController* controller, CameraTransitionStep* step) {}

protected:
    bool shouldRunOnStepStarted(int triggerMoment) const
    {
        return static_cast<CameraTransitionStepActionTrigger>(triggerMoment) == CameraTransitionStepActionTrigger::StepStarted;
    }

    bool shouldRunOnStepReached(int triggerMoment) const
    {
        return static_cast<CameraTransitionStepActionTrigger>(triggerMoment) == CameraTransitionStepActionTrigger::StepReached;
    }

    bool shouldRunOnStepFinished(int triggerMoment) const
    {
        return static_cast<CameraTransitionStepActionTrigger>(triggerMoment) == CameraTransitionStepActionTrigger::StepFinished;
    }
};
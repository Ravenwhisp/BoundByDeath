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

    ScriptFieldList getExposedFields() const override;

    virtual void onStepStarted(CameraTransitionController* controller, CameraTransitionStep* step) {}

    virtual void onStepReached(CameraTransitionController* controller, CameraTransitionStep* step) {}

    virtual void onStepFinished(CameraTransitionController* controller, CameraTransitionStep* step) {}

protected:
    bool shouldRunOnStepStarted() const
    {
        return static_cast<CameraTransitionStepActionTrigger>(m_triggerMoment) == CameraTransitionStepActionTrigger::StepStarted;
    }

    bool shouldRunOnStepReached() const
    {
        return static_cast<CameraTransitionStepActionTrigger>(m_triggerMoment) == CameraTransitionStepActionTrigger::StepReached;
    }

    bool shouldRunOnStepFinished() const
    {
        return static_cast<CameraTransitionStepActionTrigger>(m_triggerMoment) == CameraTransitionStepActionTrigger::StepFinished;
    }

public:
    int m_triggerMoment = static_cast<int>(CameraTransitionStepActionTrigger::StepReached);
};
#pragma once

#include "ScriptAPI.h"

class CameraTransitionController;
class CameraTransitionStep;

class CameraTransitionStepAction : public Script
{
public:
    explicit CameraTransitionStepAction(GameObject* owner)
        : Script(owner)
    {
    }

    virtual void onStepStarted(CameraTransitionController* controller, CameraTransitionStep* step)
    {
    }

    virtual void onStepReached(CameraTransitionController* controller, CameraTransitionStep* step)
    {
    }

    virtual void onStepFinished(CameraTransitionController* controller, CameraTransitionStep* step)
    {
    }
};
#include "pch.h"
#include "CameraTransitionStepAction.h"

static const char* cameraStepActionTriggerNames[] =
{
    "Step Started",
    "Step Reached",
    "Step Finished"
};

constexpr int cameraStepActionTriggerCount = 3;

IMPLEMENT_SCRIPT_FIELDS(CameraTransitionStepAction,
    SERIALIZED_ENUM_INT(m_triggerMoment, "Trigger Moment", cameraStepActionTriggerNames, cameraStepActionTriggerCount)
)
#include "pch.h"
#include "CameraTransitionStep.h"

static const char* cameraStepMoveModeNames[] =
{
    "Linear",
    "Smooth",
    "Catmull Rom"
};

constexpr int cameraStepMoveModeCount = 3;

IMPLEMENT_SCRIPT_FIELDS(CameraTransitionStep,
    SERIALIZED_ENUM_INT(m_moveMode, "Move Mode", cameraStepMoveModeNames, cameraStepMoveModeCount),

    SERIALIZED_FLOAT(m_moveDuration, "Move Duration", 0.0f, 20.0f, 0.05f),
    SERIALIZED_FLOAT(m_holdDuration, "Hold Duration", 0.0f, 20.0f, 0.05f),

    SERIALIZED_BOOL(m_useFovTransition, "Use FOV Transition"),
    SERIALIZED_FLOAT(m_targetFov, "Target FOV", 5.0f, 120.0f, 0.1f)
)

CameraTransitionStep::CameraTransitionStep(GameObject* owner)
    : Script(owner)
{
}

IMPLEMENT_SCRIPT(CameraTransitionStep)
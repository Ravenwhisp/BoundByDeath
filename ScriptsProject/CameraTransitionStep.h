#pragma once

#include "ScriptAPI.h"

enum class CameraStepMoveMode
{
    Linear = 0,
    Smooth,
    CatmullRom
};

class CameraTransitionStep : public Script
{
    DECLARE_SCRIPT(CameraTransitionStep)

public:
    explicit CameraTransitionStep(GameObject* owner);

    ScriptFieldList getExposedFields() const override;

    CameraStepMoveMode getMoveMode() const { return static_cast<CameraStepMoveMode>(m_moveMode); }

    float getMoveDuration() const { return m_moveDuration; }
    float getHoldDuration() const { return m_holdDuration; }

    bool usesFovTransition() const { return m_useFovTransition; }
    float getTargetFov() const { return m_targetFov; }

public:
    int m_moveMode = static_cast<int>(CameraStepMoveMode::Smooth);

    float m_moveDuration = 1.0f;
    float m_holdDuration = 0.0f;

    bool m_useFovTransition = false;
    float m_targetFov = 90.0f;
};
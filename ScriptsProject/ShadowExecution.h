#pragma once

#include "ScriptAPI.h"

class ReaperGauge;

// Shared co-op ability triggered when both players press the top face button
// (Triangle / Y) within m_timeWindow seconds of each other.
// Requires the ReaperGauge to be completely full to activate.
// Lives on the GameController GameObject alongside ReaperGauge.

class ShadowExecution : public Script
{
    DECLARE_SCRIPT(ShadowExecution)

public:
    explicit ShadowExecution(GameObject* owner);

    void Start()  override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

    bool isActive() const { return m_isActive; }

public:
    float m_timeWindow = 2.0f;  // seconds within which both players must press

private:
    void tryTrigger();

    ReaperGauge* m_reaperGauge = nullptr;

    float m_p0Timer  = 0.0f;   // countdown for player 0 press window
    float m_p1Timer  = 0.0f;   // countdown for player 1 press window
    bool  m_isActive = false;
};

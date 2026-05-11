#pragma once

#include "ScriptAPI.h"

class ReaperGauge : public Script
{
    DECLARE_SCRIPT(ReaperGauge)

public:
    explicit ReaperGauge(GameObject* owner);

    void Start()     override;
    void Update()    override;
    void drawGizmo() override;

    ScriptFieldList getExposedFields() const override;

    void  onMarkExploited();
    void  consume();
    float getGauge()        const { return m_gauge; }
    float getGaugePercent() const;
    int   getCurrentSegments() const;
    bool  isFull()          const { return m_gauge >= m_maxGauge; }

public:
    float m_maxGauge         = 100.0f;
    int   m_numSegments      = 5;
    float m_gainPerExploit   = 12.5f;
    float m_gracePeriod      = 10.0f;
    float m_decayPerSecond   = 2.0f;

private:
    float m_gauge         = 0.0f;
    float m_decayTimer    = 0.0f;
    bool  m_everExploited = false;
    bool  m_decaying      = false;
};

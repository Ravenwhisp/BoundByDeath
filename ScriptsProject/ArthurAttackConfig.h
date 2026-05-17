#pragma once

#include "ScriptAPI.h"

class ArthurAttackConfig : public Script
{
    DECLARE_SCRIPT(ArthurAttackConfig)

public:
    explicit ArthurAttackConfig(GameObject* owner);

    ScriptFieldList getExposedFields() const override;

public:
    float m_heavySwipeDamage = 10.0f;
    float m_heavySwipeRange = 3.0f;
    float m_heavySwipeHalfAngleDegrees = 60.0f;

    float m_heavySwipeTotalDuration = 1.5f;
    float m_heavySwipeHit1Time = 0.45f;
    float m_heavySwipeHit2Time = 0.95f;
    float m_heavySwipeHit3Time = 1.35f;

    float m_heavySwipeRecoveryDuration = 0.75f;
};
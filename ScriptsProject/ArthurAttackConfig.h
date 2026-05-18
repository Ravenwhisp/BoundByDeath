#pragma once

#include "ScriptAPI.h"

class ArthurAttackConfig : public Script
{
    DECLARE_SCRIPT(ArthurAttackConfig)

public:
    explicit ArthurAttackConfig(GameObject* owner);

    ScriptFieldList getExposedFields() const override;

public:
    // Heavy Swipe
    float m_heavySwipeDamage = 10.0f;
    float m_heavySwipeRange = 3.0f;
    float m_heavySwipeHalfAngleDegrees = 60.0f;

    float m_heavySwipeTotalDuration = 1.5f;
    float m_heavySwipeHit1Time = 0.45f;
    float m_heavySwipeHit2Time = 0.95f;
    float m_heavySwipeHit3Time = 1.35f;

    float m_heavySwipeRecoveryDuration = 0.75f;

    // Side Sweep
    float m_sideSweepDamage = 12.0f;
    float m_sideSweepRange = 4.0f;
    float m_sideSweepHalfAngleDegrees = 55.0f;

    float m_sideSweepHitTime = 0.35f;
    float m_sideSweepTotalDuration = 0.7f;

    float m_sideSweepRecoveryDuration = 0.5f;

    // Charging Slam

    // Earth Hammer
    float m_earthHammerDamage = 20.0f;
    float m_earthHammerRadius = 5.0f;

    float m_earthHammerTotalDuration = 2.2f;
    float m_earthHammerImpactTime = 2.0f;

    float m_earthHammerRecoveryDuration = 1.0f;
};
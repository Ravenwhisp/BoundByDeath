#include "pch.h"
#include "ArthurAttackConfig.h"

IMPLEMENT_SCRIPT_FIELDS(ArthurAttackConfig,
    // Heavy Swipe
    SERIALIZED_FLOAT(m_heavySwipeDamage, "Heavy Swipe Damage", 0.0f, 9999.0f, 1.0f),
    SERIALIZED_FLOAT(m_heavySwipeRange, "Heavy Swipe Range", 0.0f, 20.0f, 0.1f),
    SERIALIZED_FLOAT(m_heavySwipeHalfAngleDegrees, "Heavy Swipe Half Angle Degrees", 0.0f, 180.0f, 1.0f),
    SERIALIZED_FLOAT(m_heavySwipeTotalDuration, "Heavy Swipe Total Duration", 0.1f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_heavySwipeHit1Time, "Heavy Swipe Hit 1 Time", 0.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_heavySwipeHit2Time, "Heavy Swipe Hit 2 Time", 0.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_heavySwipeHit3Time, "Heavy Swipe Hit 3 Time", 0.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_heavySwipeRecoveryDuration, "Heavy Swipe Recovery Duration", 0.0f, 10.0f, 0.05f),
    // Side Sweep
    SERIALIZED_FLOAT(m_sideSweepDamage, "Side Sweep Damage", 0.0f, 9999.0f, 1.0f),
    SERIALIZED_FLOAT(m_sideSweepRange, "Side Sweep Range", 0.0f, 30.0f, 0.1f),
    SERIALIZED_FLOAT(m_sideSweepHalfAngleDegrees, "Side Sweep Half Angle Degrees", 0.0f, 180.0f, 1.0f),
    SERIALIZED_FLOAT(m_sideSweepHitTime, "Side Sweep Hit Time", 0.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_sideSweepTotalDuration, "Side Sweep Total Duration", 0.1f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_sideSweepRecoveryDuration, "Side Sweep Recovery Duration", 0.0f, 10.0f, 0.05f),
   
    // Charging Slam

    // Earth Hammer
    SERIALIZED_FLOAT(m_earthHammerDamage, "Earth Hammer Damage", 0.0f, 9999.0f, 1.0f),
    SERIALIZED_FLOAT(m_earthHammerRadius, "Earth Hammer Radius", 0.0f, 30.0f, 0.1f),
    SERIALIZED_FLOAT(m_earthHammerTotalDuration, "Earth Hammer Total Duration", 0.1f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_earthHammerImpactTime, "Earth Hammer Impact Time", 0.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_earthHammerRecoveryDuration, "Earth Hammer Recovery Duration", 0.0f, 10.0f, 0.05f)
)

ArthurAttackConfig::ArthurAttackConfig(GameObject* owner)
    : Script(owner)
{
}

IMPLEMENT_SCRIPT(ArthurAttackConfig)
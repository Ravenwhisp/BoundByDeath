#include "pch.h"
#include "ArthurAttackConfig.h"

IMPLEMENT_SCRIPT_FIELDS(ArthurAttackConfig,
    // Heavy Swipe
    SERIALIZED_FLOAT(m_heavySwipeDamage, "Heavy Swipe Damage", 0.0f, 9999.0f, 1.0f),
    SERIALIZED_FLOAT(m_heavySwipeRange, "Heavy Swipe Range", 0.0f, 20.0f, 0.1f),
    SERIALIZED_FLOAT(m_heavySwipeHalfAngleDegrees, "Heavy Swipe Half Angle Degrees", 0.0f, 180.0f, 1.0f),
    SERIALIZED_FLOAT(m_heavySwipeTotalDuration, "Heavy Swipe Total Duration", 0.1f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_heavySwipeHit1ChargingDuration, "Heavy Swipe Hit 1 Charging Duration", 0.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_heavySwipeHit2ChargingDuration, "Heavy Swipe Hit 2 Charging Duration", 0.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_heavySwipeHit3ChargingDuration, "Heavy Swipe Hit 3 Charging Duration", 0.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_heavySwipeRecoveryDuration, "Heavy Swipe Recovery Duration", 0.0f, 10.0f, 0.05f),
    // Side Sweep
    SERIALIZED_FLOAT(m_sideSweepDamage, "Side Sweep Damage", 0.0f, 9999.0f, 1.0f),
    SERIALIZED_FLOAT(m_sideSweepRange, "Side Sweep Range", 0.0f, 30.0f, 0.1f),
    SERIALIZED_FLOAT(m_sideSweepHalfAngleDegrees, "Side Sweep Half Angle Degrees", 0.0f, 180.0f, 1.0f),
    SERIALIZED_FLOAT(m_sideSweepChargingDuration, "Side Sweep Charging Duration", 0.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_sideSweepTotalDuration, "Side Sweep Total Duration", 0.1f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_sideSweepRecoveryDuration, "Side Sweep Recovery Duration", 0.0f, 10.0f, 0.05f),
    // Charging Slam
    SERIALIZED_FLOAT(m_chargingSlamDashDamage, "Charging Slam Dash Damage", 0.0f, 9999.0f, 1.0f),
    SERIALIZED_FLOAT(m_chargingSlamFinalAreaImpactDamage, "Charging Slam Final Area Impact Damage", 0.0f, 9999.0f, 1.0f),
    SERIALIZED_FLOAT(m_chargingSlamChargingDuration, "Charging Slam Charging Duration", 0.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_chargingSlamDashSpeed, "Charging Slam Dash Speed", 0.0f, 50.0f, 0.1f),
    SERIALIZED_FLOAT(m_chargingSlamDashHitRadius, "Charging Slam Dash Hit Radius", 0.0f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_chargingSlamImpactRadius, "Charging Slam Impact Radius", 0.0f, 20.0f, 0.1f),
    SERIALIZED_FLOAT(m_chargingSlamTotalDuration, "Charging Slam Total Duration", 0.1f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_chargingSlamRecoveryDuration, "Charging Slam Recovery Duration", 0.0f, 10.0f, 0.05f),
    // Earth Hammer
    SERIALIZED_FLOAT(m_earthHammerDamage, "Earth Hammer Damage", 0.0f, 9999.0f, 1.0f),
    SERIALIZED_FLOAT(m_earthHammerRadius, "Earth Hammer Radius", 0.0f, 30.0f, 0.1f),
    SERIALIZED_FLOAT(m_earthHammerTotalDuration, "Earth Hammer Total Duration", 0.1f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_earthHammerChargingDuration, "Earth Hammer Charging Duration", 0.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_earthHammerRecoveryDuration, "Earth Hammer Recovery Duration", 0.0f, 10.0f, 0.05f)
)

ArthurAttackConfig::ArthurAttackConfig(GameObject* owner)
    : Script(owner)
{
}

IMPLEMENT_SCRIPT(ArthurAttackConfig)
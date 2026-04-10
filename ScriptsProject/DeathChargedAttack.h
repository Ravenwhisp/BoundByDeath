#pragma once

#include "AbilityBase.h"

// Death's charged attack — hold RT / R2 to charge, release to strike (RB / R1).
// Sibling script of DeathCharacter on the same GameObject.
// Reads m_chargedAttackDamage from DeathCharacter via static_cast<DeathCharacter*>(m_character).
//
// isActive() (from AbilityBase) reflects whether the charge is currently held.
// The attack is only executed if the button is released after m_minChargeTime seconds.
class DeathChargedAttack : public AbilityBase
{
    DECLARE_SCRIPT(DeathChargedAttack)

public:
    explicit DeathChargedAttack(GameObject* owner);

    void Start()  override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

public:
    // Minimum hold time (seconds) required for the strike to fire on release.
    float m_minChargeTime = 0.5f;

private:
    // Time accumulated while the trigger is held.
    float m_chargeTime = 0.0f;
};

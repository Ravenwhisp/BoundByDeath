#pragma once

#include "DeathAbilityBase.h"

// Death's heavy attack — R2 (RT) with two modes:
//
//   Quick press  (release before m_minChargeTime):
//     Deals m_chargedAttackDamage to all enemies in the attack arc.
//     Can be used up to twice consecutively in a combo (enforced by DeathCharacter).
//
//   Charged press (hold >= m_minChargeTime, release to fire):
//     Scales damage from 1x to 2x m_chargedAttackDamage based on how close
//     the charge is to m_maxChargeTime.
//     COMBO STARTER ONLY — ignored if already mid-combo (comboStep > 0).
//     After landing, the combo continues normally.
class DeathChargedAttack : public DeathAbilityBase
{
    DECLARE_SCRIPT(DeathChargedAttack)

public:
    explicit DeathChargedAttack(GameObject* owner);

    void Start()  override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

public:
    float m_minChargeTime = 0.5f;

private:
    float m_chargeTime  = 0.0f;
    bool  m_isCharging  = false;
};

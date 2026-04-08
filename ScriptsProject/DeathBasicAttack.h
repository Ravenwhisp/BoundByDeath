#pragma once

#include "AbilityBase.h"

// Death's basic attack — up to a 3-hit melee combo (RB / R1).
// Sibling script of DeathCharacter on the same GameObject.
// Reads m_basicAttackDamage from DeathCharacter via static_cast<DeathCharacter*>(m_character).
//
// Each individual hit is an instant activate → deactivate so canAct is only
// blocked for a single frame per hit, allowing the player to chain inputs freely.
// The combo state is tracked independently via m_comboStep and m_comboTimer.
class DeathBasicAttack : public AbilityBase
{
    DECLARE_SCRIPT(DeathBasicAttack)

public:
    explicit DeathBasicAttack(GameObject* owner);

    void Start()  override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

public:
    // Time window (seconds) within which the next combo hit must be input.
    // Exposed to the inspector so designers can tune the feel.
    float m_comboWindow = 1.0f;

private:
    // Current hit in the combo sequence: 0 = idle, 1 = first hit, 2 = second, 3 = third.
    // Resets to 0 after the third hit or when m_comboTimer exceeds m_comboWindow.
    int   m_comboStep  = 0;

    // Time elapsed since the last hit. Resets on each successful hit input.
    float m_comboTimer = 0.0f;
};

#pragma once

#include "AbilityBase.h"

// Death's light attack — chains up to 3 hits (R1 / RB).
// Sibling script of DeathCharacter on the same GameObject.
//
// Each hit is instant (activate → deactivate in the same frame), so canAct
// is only blocked for a single frame, allowing fluid combo input.
//
// Combo state (step, consecutive R2 count, window timer) is shared with
// DeathChargedAttack and lives in DeathCharacter.
//
// Every hit deals damage to ALL enemies within the character's attack arc
// (m_arcRange, m_arcAngle on DeathCharacter).
class DeathBasicAttack : public AbilityBase
{
    DECLARE_SCRIPT(DeathBasicAttack)

public:
    explicit DeathBasicAttack(GameObject* owner);

    void Start()      override;
    void Update()     override;
    void drawGizmo()  override;

    ScriptFieldList getExposedFields() const override;
};

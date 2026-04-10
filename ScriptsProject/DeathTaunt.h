#pragma once

#include "AbilityBase.h"

// Death's taunt — provokes nearby enemies for a duration (LT / L2).
// Sibling script of DeathCharacter on the same GameObject.
// Reads m_tauntDuration from DeathCharacter via static_cast<DeathCharacter*>(m_character).
//
// isActive() (from AbilityBase) reflects whether the taunt is currently active.
// The exact effect on enemies (aggro, debuff, etc.) is pending design definition.
// TODO: define taunt mechanic — what does it actually do to enemies?
class DeathTaunt : public AbilityBase
{
    DECLARE_SCRIPT(DeathTaunt)

public:
    explicit DeathTaunt(GameObject* owner);

    void Start()  override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

private:
    // Time elapsed since the taunt began.
    float m_tauntTimer = 0.0f;
};

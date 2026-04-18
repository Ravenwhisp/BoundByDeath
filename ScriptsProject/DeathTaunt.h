#pragma once

#include "DeathAbilityBase.h"

// Death's taunt — provokes nearby enemies for a duration (LT / L2).
// Reads m_tauntDuration from DeathCharacter via m_deathChar.
// TODO: define taunt mechanic — what does it actually do to enemies?
class DeathTaunt : public DeathAbilityBase
{
    DECLARE_SCRIPT(DeathTaunt)

public:
    explicit DeathTaunt(GameObject* owner);

    void Start()  override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

private:
    float m_tauntTimer  = 0.0f;
    bool  m_isTaunting  = false;
};

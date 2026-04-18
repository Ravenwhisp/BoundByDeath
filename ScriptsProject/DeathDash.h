#pragma once

#include "DeathAbilityBase.h"

// Death's aggressive dash — launches towards the current target (LB / L1).
// If there is no target, dashes in the character's forward direction instead.
// Reads m_dashDistance from DeathCharacter via m_deathChar.
class DeathDash : public DeathAbilityBase
{
    DECLARE_SCRIPT(DeathDash)

public:
    explicit DeathDash(GameObject* owner);

    void Start()  override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

public:
    float m_dashDuration = 0.2f;

private:
    float   m_dashTimer     = 0.0f;
    bool    m_isDashing     = false;
    Vector3 m_dashDirection = { 0.0f, 0.0f, 0.0f };
};

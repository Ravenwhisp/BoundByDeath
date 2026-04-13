#pragma once

#include "AbilityBase.h"

// Death's aggressive dash — launches towards the current target (LB / L1).
// If there is no target, dashes in the character's forward direction instead.
// Sibling script of DeathCharacter on the same GameObject.
// Reads m_dashDistance from DeathCharacter via static_cast<DeathCharacter*>(m_character).
//
// isActive() (from AbilityBase) reflects whether the dash is currently in motion.
// Movement is applied over m_dashDuration seconds then onDeactivate() is called.
class DeathDash : public AbilityBase
{
    DECLARE_SCRIPT(DeathDash)

public:
    explicit DeathDash(GameObject* owner);

    void Start()  override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

public:
    // Total time (seconds) the dash movement lasts.
    float m_dashDuration = 0.2f;
    float m_dashDistance = 3.0f;

private:
    // Time elapsed since the dash began.
    float m_dashTimer = 0.0f;

    // Direction calculated at the moment of activation, used throughout the dash.
    Vector3 m_dashDirection = { 0.0f, 0.0f, 0.0f };

    bool m_debugDashKeyWasDown = false;
};

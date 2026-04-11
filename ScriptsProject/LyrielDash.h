#pragma once

#include "AbilityBase.h"

// Lyriel's evasive dash — moves away from danger (LB / L1).
// Unlike Death's aggressive dash (towards the target), this one moves in the
// direction of the left stick input, or backwards if no input is given.
// Sibling script of LyrielCharacter on the same GameObject.
// Reads m_dashDistance from LyrielCharacter via static_cast<LyrielCharacter*>(m_character).
//
// isActive() (from AbilityBase) reflects whether the dash is currently in motion.
class LyrielDash : public AbilityBase
{
    DECLARE_SCRIPT(LyrielDash)

public:
    explicit LyrielDash(GameObject* owner);

    void Start()  override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

public:
    // Total time (seconds) the dash movement lasts.
    float m_dashDuration = 0.15f;

private:
    // Time elapsed since the dash began.
    float m_dashTimer = 0.0f;

    // Direction calculated at the moment of activation, used throughout the dash.
    Vector3 m_dashDirection = { 0.0f, 0.0f, 0.0f };
};

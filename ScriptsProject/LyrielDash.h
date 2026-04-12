#pragma once

#include "AbilityBase.h"

#define MAX_CHARGES 3

class PlayerController;
class PlayerMovement;

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
	void drawGizmo() override;
    
    void recoverCharge();

    ScriptFieldList getExposedFields() const override;

public:
    // Total time (seconds) the dash movement lasts.
    float m_dashDuration = 0.15f;
	float m_dashDistance = 3.0f;

	bool m_debugEnabled = false;

protected:
	bool canActivate() const override;
	void onActivate() override;
	void onDeactivate() override;

private:
	PlayerController* m_playerController = nullptr;
	PlayerMovement* m_playerMovement = nullptr;
    // Time elapsed since the dash began.
    float m_dashTimer = 0.0f;
	float m_cooldownTimer = 0.0f;

	int m_charges = MAX_CHARGES;

	bool m_isDashing = false;

    // Direction calculated at the moment of activation, used throughout the dash.
    Vector3 m_dashDirection = { 0.0f, 0.0f, 0.0f };

private:
    inline void addCharge()
    {
		++m_charges;
		if (m_charges == MAX_CHARGES)
		{
			m_cooldownTimer = 0.0f;
		}
	}

	void calculateDashMovement(const float dt);

	PlayerController* findControllerScript(GameObject* owner) const;
	PlayerMovement* findMovementScript(GameObject* owner) const;
};

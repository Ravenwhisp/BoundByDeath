#pragma once

#include "ScriptAPI.h"
#include "IDash.h"

#define MAX_CHARGES 3

class PlayerMovement;

class LyrielDash : public IDash
{
    DECLARE_SCRIPT(LyrielDash)

public:
    explicit LyrielDash(GameObject* owner);

    void Start() override;
    void Update() override;
	void drawGizmo() override;

    ScriptFieldList getExposedFields() const override;

	bool canDash() const override;
	bool isDashing() const override { return m_isDashing; }
	void startDash(const Vector3& direction	) override;

	void recoverChargeDash();

	void endDash();

	float m_dashDuration = 0.5f;
	float m_dashDistance = 3.0f;
	//float m_dashSpeed = 20.0f; //quitar

	bool m_debugEnabled = false;

private:
	PlayerMovement* m_movement = nullptr;

	bool m_isDashing = false;

	float m_dashTimer = 0.0f;
	float m_cooldownTimer = 0.0f;

	int m_dashCharges = 3;

private:
	inline void addCharge()
	{
		++m_dashCharges;
		if (m_dashCharges == MAX_CHARGES)
		{
			m_cooldownTimer = 0.0f;
		}
	}

	void calculateDashMovement(const float dt);

	PlayerMovement* findMovementScript(GameObject* owner);

};
#pragma once

#include "ScriptAPI.h"

class PaladinAttackConfig : public Script
{
	DECLARE_SCRIPT(PaladinAttackConfig)

public:
	explicit PaladinAttackConfig(GameObject* owner);

	ScriptFieldList getExposedFields() const override;

public:
	// Basic attack
	float m_basicAttackRange = 2.0f;
	float m_basicAttackDamage = 10.0f;
	float m_basicAttackWindupTime = 0.35f;
	float m_basicAttackTotalDuration = 0.8f;
	float m_basicAttackCooldown = 1.2f;

	// Charge
	float m_chargeRange = 5.0f;
	float m_chargeDuration = 0.5f;
	float m_chargeSpeed = 6.0f;
	float m_chargeCooldown = 3.0f;
};
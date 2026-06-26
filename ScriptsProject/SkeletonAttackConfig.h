#pragma once

#include "EnemyBaseAttackConfig.h"

class SkeletonAttackConfig : public EnemyBaseAttackConfig
{
	DECLARE_SCRIPT(SkeletonAttackConfig)

public:
	explicit SkeletonAttackConfig(GameObject* owner);

	ScriptFieldList getExposedFields() const override;

public:
	// --- Scimitar Strike --- //
	// Dash
	float m_scimitarDashDuration = 0.2f;
	float m_scimitarDashSpeed = 5.0f;

	// Attack
	float m_attackClipDuration = 1.33f;
	float m_attackHitTime = 0.45f;
	float m_attackAnimationBlendTime = 0.05f;

	// Backstep
	float m_stepBackDuration = 0.25f;
	float m_stepBackSpeed = 2.0f;

	// Final Strike
	float m_scimitarStunDuration = 0.5f;

	// --- Guard --- //
	float m_guardDuration = 5.0f;
	float m_guardCooldown = 3.0f;
	float m_guardRange = 3.5f;
	float m_guardHealPerSecond = 5.0f;
	float m_guardBlockHalfAngleDegrees = 60.0f;
};
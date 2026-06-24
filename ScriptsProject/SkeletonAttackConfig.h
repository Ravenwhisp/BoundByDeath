#pragma once

#include "EnemyBaseAttackConfig.h"

class SkeletonAttackConfig : public EnemyBaseAttackConfig
{
	DECLARE_SCRIPT(SkeletonAttackConfig)

public:
	explicit SkeletonAttackConfig(GameObject* owner);

	ScriptFieldList getExposedFields() const override;

public:
	// Scimitar Strike
	float m_scimitarDashDuration = 0.2f;
	float m_scimitarDashSpeed = 5.0f;

	float m_firstHitTime = 0.35f;
	float m_stepBackDuration = 0.25f;
	float m_stepBackSpeed = 2.0f;

	float m_secondHitTime = 0.85f;
	float m_scimitarTotalDuration = 1.2f;

	float m_scimitarStunDuration = 0.5f;

	// Guard
	float m_guardDuration = 5.0f;
	float m_guardCooldown = 3.0f;
	float m_guardRange = 3.5f;
	float m_guardHealPerSecond = 5.0f;
	float m_guardHalfAngleDegrees = 60.0f;
};
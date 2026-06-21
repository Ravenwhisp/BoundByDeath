#pragma once

#include "EnemyBaseAttackConfig.h"

class SummonerAttackConfig : public EnemyBaseAttackConfig
{
	DECLARE_SCRIPT(SummonerAttackConfig)

public:
	explicit SummonerAttackConfig(GameObject* owner);

	ScriptFieldList getExposedFields() const override;

public:
	// Teleport
	float m_teleportCooldown = 12.0f;
	float m_teleportRadius = 6.0f;

	// Energy Ball
	float m_energyBallSpeed = 8.0f;

	// Summon
	float m_summonCooldown = 8.0f;
	float m_summonRecoverDuration = 3.0f;
};
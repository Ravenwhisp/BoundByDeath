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
	float m_teleportCooldown = 5.0f;

	// Energy Ball
	float m_energyBallRange = 8.0f;
	float m_energyBallDamage = 25.0f;
	float m_energyBallCooldown = 4.0f;

	// Summon
	float m_summonCooldown = 8.0f;
	float m_summonRecoverDuration = 3.0f;
};
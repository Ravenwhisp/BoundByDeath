#include "pch.h"
#include "SummonerAttackConfig.h"

IMPLEMENT_SCRIPT_FIELDS_INHERITED(SummonerAttackConfig, EnemyBaseAttackConfig,
	// Teleport
	FIELD_GROUP_COLLAPSE("Teleport",
		SERIALIZED_FLOAT(m_teleportCooldown, "Teleport Cooldown", 0.0f, 30.0f, 0.1f)
	),
	// Energy Ball
	FIELD_GROUP_COLLAPSE("Energy Ball", 
		SERIALIZED_FLOAT(m_energyBallRange, "Energy Ball Range", 0.0f, 100.0f, 1.0f),
		SERIALIZED_FLOAT(m_energyBallDamage, "Energy Ball Damage", 0.0f, 50.0, 1.0f),
		SERIALIZED_FLOAT(m_energyBallCooldown, "Energy Ball Cooldown", 0.0f, 30.0f, 0.1f)
	),
	// Summon
	FIELD_GROUP_COLLAPSE("Summon",
		SERIALIZED_FLOAT(m_summonCooldown, "Summon Cooldown", 0.0f, 30.0f, 0.1f),
		SERIALIZED_FLOAT(m_summonRecoverDuration, "Summon Recover Duration", 0.0f, 10.0f, 0.1f)
	)
)

SummonerAttackConfig::SummonerAttackConfig(GameObject* owner)
	: EnemyBaseAttackConfig(owner)
{
}

IMPLEMENT_SCRIPT(SummonerAttackConfig)
#include "pch.h"
#include "PaladinAttackConfig.h"

IMPLEMENT_SCRIPT_FIELDS(PaladinAttackConfig,
	// Basic Attack
	FIELD_GROUP_COLLAPSE("Basic Attack",
		SERIALIZED_FLOAT(m_basicAttackRange, "Basic Attack Range", 0.0f, 100.0f, 0.1f),
		SERIALIZED_FLOAT(m_basicAttackDamage, "Basic Attack Damage", 0.0f, 9999.0f, 1.0f),
		SERIALIZED_FLOAT(m_basicAttackWindupTime, "Basic Attack Windup Time", 0.0f, 10.0f, 0.05f),
		SERIALIZED_FLOAT(m_basicAttackTotalDuration, "Basic Attack Total Duration", 0.1f, 10.0f, 0.05f),
		SERIALIZED_FLOAT(m_basicAttackCooldown, "Basic Attack Cooldown", 0.0f, 10.0f, 0.05f)
	),
	// Charge
	FIELD_GROUP_COLLAPSE("Charge",
		SERIALIZED_FLOAT(m_chargeRange, "Charge Range", 0.0f, 20.0f, 0.1f),
		SERIALIZED_FLOAT(m_chargeDuration, "Charge Duration", 0.0f, 10.0f, 0.05f),
		SERIALIZED_FLOAT(m_chargeSpeed, "Charge Speed", 0.0f, 10.0f, 0.05f),
		SERIALIZED_FLOAT(m_chargeCooldown, "Charge Cooldown", 0.0f, 10.0f, 0.05f)
	)
)

PaladinAttackConfig::PaladinAttackConfig(GameObject* owner)
	: Script(owner)
{
}

IMPLEMENT_SCRIPT(PaladinAttackConfig)
#include "pch.h"
#include "SkeletonAttackConfig.h"

IMPLEMENT_SCRIPT_FIELDS_INHERITED(SkeletonAttackConfig, EnemyBaseAttackConfig,
	// Scimitar Strike
	FIELD_GROUP_COLLAPSE("Scimitar Strike",
		SERIALIZED_FLOAT(m_scimitarDashDuration, "Scimitar Dash Duration", 0.0f, 5.0f, 0.1f),
		SERIALIZED_FLOAT(m_scimitarDashSpeed, "Scimitar Dash Speed", 0.0f, 20.0f, 0.1f),
		SERIALIZED_FLOAT(m_firstHitTime, "First Hit Time", 0.0f, 5.0f, 0.05f),
		SERIALIZED_FLOAT(m_stepBackDuration, "Step Back Duration", 0.0f, 5.0f, 0.1f),
		SERIALIZED_FLOAT(m_stepBackSpeed, "Step Back Speed", 0.0f, 20.0f, 0.1f),
		SERIALIZED_FLOAT(m_secondHitTime, "Second Hit Time", 0.0f, 5.0f, 0.1f),
		SERIALIZED_FLOAT(m_scimitarTotalDuration, "Scimitar Total Duration", 0.1f, 10.0f, 0.05f),
		SERIALIZED_FLOAT(m_scimitarStunDuration, "Scimitar Stun Duration", 0.0f, 5.0f, 0.1f)
	),
	// Guard
	FIELD_GROUP_COLLAPSE("Guard",
		SERIALIZED_FLOAT(m_guardDuration, "Guard Duration", 0.0f, 10.0f, 0.1f),
		SERIALIZED_FLOAT(m_guardCooldown, "Guard Cooldown", 0.0f, 10.0f, 0.1f),
		SERIALIZED_FLOAT(m_guardRange, "Guard Range", 0.0f, 20.0f, 0.1f),
		SERIALIZED_FLOAT(m_guardHealPerSecond, "Guard Heal Per Second", 0.0f, 100.0f, 1.0f),
		SERIALIZED_FLOAT(m_guardHalfAngleDegrees, "Guard Half Angle Degrees", 0.0f, 180.0f, 1.0f)
	)
)

SkeletonAttackConfig::SkeletonAttackConfig(GameObject* owner)
	: EnemyBaseAttackConfig(owner)
{
}

IMPLEMENT_SCRIPT(SkeletonAttackConfig)
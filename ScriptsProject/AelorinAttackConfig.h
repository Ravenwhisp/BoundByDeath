#pragma once

#include "DataContainerAPI.h"

class AelorinAttackConfig : public DataContainer
{
	DECLARE_DATACONTAINER(AelorinAttackConfig)

public:
	AelorinAttackConfig() = default;

	explicit AelorinAttackConfig(AssetId& id)
		: DataContainer(id)
	{
	}

	// Thresholds
	float m_thresholdStaggerDuration = 2.0f;

	// Fury
	int m_firstFuryCastCount = 6;
	int m_secondFuryCastCount = 10;

	// Teleport
	float m_teleportCastDuration = 3.0f;
	float m_teleportCooldown = 20.0f;

	// Summons
	int m_phase1SummonCap = 4;
	int m_phase2SummonCap = 5;

	IMPLEMENT_DATACONTAINER_FIELDS(
		AelorinAttackConfig,

		FIELD_GROUP_COLLAPSE("Thresholds",
			SERIALIZED_FLOAT(m_thresholdStaggerDuration, "Threshold Stagger Duration", 0.0f, 10.0f, 0.05f)
		),

		FIELD_GROUP_COLLAPSE("Fury",
			SERIALIZED_INT(m_firstFuryCastCount, "First Fury Cast Count", 0, 50, 1),
			SERIALIZED_INT(m_secondFuryCastCount, "Second Fury Cast Count", 0, 50, 1)
		),

		FIELD_GROUP_COLLAPSE("Teleport",
			SERIALIZED_FLOAT(m_teleportCastDuration, "Teleport Cast Duration", 0.0f, 10.0f, 0.05f),
			SERIALIZED_FLOAT(m_teleportCooldown, "Teleport Cooldown", 0.0f, 60.0f, 0.1f)
		),

		FIELD_GROUP_COLLAPSE("Summons",
			SERIALIZED_INT(m_phase1SummonCap, "Phase 1 Summon Cap", 0, 10, 1),
			SERIALIZED_INT(m_phase2SummonCap, "Phase 2 Summon Cap", 0, 10, 1)
		)
	)
};
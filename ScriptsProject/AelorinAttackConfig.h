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

	// Decision Timing
	float m_phase1DecisionTime = 2.0f;
	float m_phase2DecisionTime = 1.0f;

	// Thresholds
	float m_thresholdStaggerDuration = 2.0f;

	// Fury
	int m_firstFuryCastCount = 6;
	int m_secondFuryCastCount = 10;

	// Teleport
	float m_teleportCastTime = 3.0f;
	float m_teleportCooldown = 20.0f;

	// Summons
	int m_phase1SummonCap = 4;
	int m_phase2SummonCap = 5;

	// Health Drops
	PrefabRef m_healthPickupPrefab;
	int m_healthDropQuantity = 3;
	float m_healingAmount = 15.0f;
	float m_dropRadius = 3.0f;
	float m_dropHeight = 1.5f;

	IMPLEMENT_DATACONTAINER_FIELDS(
		AelorinAttackConfig,

		FIELD_GROUP_COLLAPSE("Decision Timing",
			SERIALIZED_FLOAT(m_phase1DecisionTime, "Phase 1 Decision Time", 0.0f, 10.0f, 0.05f),
			SERIALIZED_FLOAT(m_phase2DecisionTime, "Phase 2 Decision Time", 0.0f, 10.0f, 0.05f)
		),

		FIELD_GROUP_COLLAPSE("Thresholds",
			SERIALIZED_FLOAT(m_thresholdStaggerDuration, "Threshold Stagger Duration", 0.0f, 10.0f, 0.05f)
		),

		FIELD_GROUP_COLLAPSE("Fury",
			SERIALIZED_INT(m_firstFuryCastCount, "First Fury Cast Count", 0, 50, 1),
			SERIALIZED_INT(m_secondFuryCastCount, "Second Fury Cast Count", 0, 50, 1)
		),

		FIELD_GROUP_COLLAPSE("Teleport",
			SERIALIZED_FLOAT(m_teleportCastTime, "Teleport Cast Time", 0.0f, 10.0f, 0.05f),
			SERIALIZED_FLOAT(m_teleportCooldown, "Teleport Cooldown", 0.0f, 60.0f, 0.1f)
		),

		FIELD_GROUP_COLLAPSE("Summons",
			SERIALIZED_INT(m_phase1SummonCap, "Phase 1 Summon Cap", 0, 10, 1),
			SERIALIZED_INT(m_phase2SummonCap, "Phase 2 Summon Cap", 0, 10, 1)
		),

		FIELD_GROUP_COLLAPSE("Health Drops",
			SERIALIZED_ASSET_REF(m_healthPickupPrefab, "Health Pickup Prefab", AssetType::PREFAB),
			SERIALIZED_INT(m_healthDropQuantity, "Health Drop Quantity", 0, 20, 1),
			SERIALIZED_FLOAT(m_healingAmount, "Healing Amount", 0.0f, 100.0f, 1.0f),
			SERIALIZED_FLOAT(m_dropRadius, "Drop Radius", 0.0f, 20.0f, 0.1f),
			SERIALIZED_FLOAT(m_dropHeight, "Drop Height", 0.0f, 10.0f, 0.1f)
		)
	)
};
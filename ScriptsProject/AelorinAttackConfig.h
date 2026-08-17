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

	// Seeker Sigils
	int m_seekerSigilsWaveCount = 3;
	
	float m_seekerSigilsInitialDelay = 0.6f;
	float m_seekerSigilsWaveInterval = 0.45f;
	float m_seekerSigilsRecoveryDuration = 2.2f;
	
	float m_seekerSigilsRadius = 1.25f;
	float m_seekerSigilsDamage = 10.0f;

	float m_seekerSigilsPhase2FinalDelay = 0.5f;
	float m_seekerSigilsPhase2FinalRadius = 2.5f;
	float m_seekerSigilsPhase2FinalDamage = 20.0f;

	float m_seekerSigilsSpawnHeight = 8.0f;
	float m_seekerSigilsFallSpeed = 10.0f;
	float m_seekerSigilsProjectileLifetime = 3.0f;

	// Nova
	float m_novaTriggerDistance = 4.0f;

	float m_novaChargeTime = 3.0f;
	float m_novaRadius = 5.5f;
	float m_novaDamage = 30.0f;

	float m_novaPhase2SecondWaveDelay = 0.45f;
	float m_novaPhase2SecondRadius = 8.0f;
	float m_novaPhase2SecondDamage = 40.0f;

	float m_novaRecoveryDuration = 1.25f;

	// Risen Spires
	float m_risenSpiresWindupDuration = 3.0f;
	float m_risenSpiresRadius = 1.5f;
	float m_risenSpiresDamage = 30.0f;
	
	float m_risenSpiresPhase2SecondPassDelay = 2.0f;
	float m_risenSpiresRecoveryDuration = 1.0f;

	// Spirit Cannon
	float m_spiritCannonWindupDuration = 3.0f;

	float m_spiritCannonBeamLength = 20.0f;
	float m_spiritCannonBeamWidth = 1.5f;
	float m_spiritCannonDamage = 30.0f;

	float m_spiritCannonPhase1ShotInterval = 0.8f;
	float m_spiritCannonPhase2ShotInterval = 0.45f;

	float m_spiritCannonPhase2FinalShotDelay = 0.8f;
	float m_spiritCannonPhase2FinalBeamWidth = 4.0f;
	float m_spiritCannonPhase2FinalDamage = 40.0f;

	float m_spiritCannonRecoveryDuration = 1.0f;

	// Grasp of the Dead
	float m_graspPullDuration = 2.0f;
	float m_graspPullStrength = 3.0f;

	// Teleport
	float m_teleportCastDuration = 3.0f;
	float m_teleportCooldownDuration = 20.0f;

	float m_teleportTriggerDistance = 4.0f;

	float m_teleportPhase2BurstRadius = 3.0f;
	float m_teleportPhase2BurstDamage = 20.0f;

	float m_teleportRecoveryDuration = 0.75f;

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

		FIELD_GROUP_COLLAPSE("Seeker Sigils",
			SERIALIZED_INT(m_seekerSigilsWaveCount, "Wave Count", 0, 10, 1),
			SERIALIZED_FLOAT(m_seekerSigilsInitialDelay, "Initial Delay", 0.0f, 5.0f, 0.01f),
			SERIALIZED_FLOAT(m_seekerSigilsWaveInterval, "Wave Interval", 0.0f, 5.0f, 0.01f),
			SERIALIZED_FLOAT(m_seekerSigilsRecoveryDuration, "Recovery Duration", 0.0f, 10.0f, 0.1f),
			SERIALIZED_FLOAT(m_seekerSigilsRadius, "Radius", 0.0f, 10.0f, 0.1f),
			SERIALIZED_FLOAT(m_seekerSigilsDamage, "Damage", 0.0f, 50.0f, 1.0f),
			SERIALIZED_FLOAT(m_seekerSigilsPhase2FinalDelay, "Phase 2 Delay", 0.0f, 5.0f, 0.01f),
			SERIALIZED_FLOAT(m_seekerSigilsPhase2FinalRadius, "Phase 2 Radius", 0.0f, 20.0f, 0.1f),
			SERIALIZED_FLOAT(m_seekerSigilsPhase2FinalDamage, "Phase 2 Damage", 0.0f, 50.0f, 1.0f),
			SERIALIZED_FLOAT(m_seekerSigilsSpawnHeight, "Spawn Height", 0.0f, 50.0f, 0.1f),
			SERIALIZED_FLOAT(m_seekerSigilsFallSpeed, "Fall Speed", 0.0f, 50.0f, 0.1f),
			SERIALIZED_FLOAT(m_seekerSigilsProjectileLifetime, "Lifetime", 0.0f, 20.0f, 0.1f)
		),

		FIELD_GROUP_COLLAPSE("Nova",
			SERIALIZED_FLOAT(m_novaTriggerDistance, "Nova Trigger Distance", 0.0f, 30.0f, 0.1f),
			SERIALIZED_FLOAT(m_novaChargeTime, "Nova Charge Time", 0.0f, 10.0f, 0.05f),
			SERIALIZED_FLOAT(m_novaRadius, "Nova Radius", 0.0f, 50.0f, 0.1f),
			SERIALIZED_FLOAT(m_novaDamage, "Nova Damage", 0.0f, 200.0f, 1.0f),
			SERIALIZED_FLOAT(m_novaPhase2SecondWaveDelay, "Nova Phase 2 Wave Delay", 0.0f, 5.0f, 0.05f),
			SERIALIZED_FLOAT(m_novaPhase2SecondRadius, "Nova Phase 2 Second Radius", 0.0f, 100.0f, 1.0f),
			SERIALIZED_FLOAT(m_novaPhase2SecondDamage, "Nova Phase 2 Second Damage", 0.0f, 300.0f, 1.0f),
			SERIALIZED_FLOAT(m_novaRecoveryDuration, "Nova Recovery Duration", 0.0f, 10.0f, 0.05f)
		),

		FIELD_GROUP_COLLAPSE("Risen Spires",
			SERIALIZED_FLOAT(m_risenSpiresWindupDuration, "Windup Duration", 0.0f, 10.0f, 0.1f),
			SERIALIZED_FLOAT(m_risenSpiresRadius, "Spire Radius", 0.0f, 10.0f, 0.1f),
			SERIALIZED_FLOAT(m_risenSpiresDamage, "Spire Damage", 0.0f,	9999.0f, 1.0f),
			SERIALIZED_FLOAT(m_risenSpiresPhase2SecondPassDelay, "Phase 2 Second Pass Delay", 0.0f, 10.0f, 0.1f),
			SERIALIZED_FLOAT(m_risenSpiresRecoveryDuration, "Recovery Duration", 0.0f, 10.0f, 0.1f)
		),

		FIELD_GROUP_COLLAPSE("Spirit Cannon",
			SERIALIZED_FLOAT(m_spiritCannonWindupDuration, "Windup Duration", 0.0f, 10.0f, 0.1f),
			SERIALIZED_FLOAT(m_spiritCannonBeamLength, "Beam Length", 0.0f, 200.0f, 1.0f),
			SERIALIZED_FLOAT(m_spiritCannonBeamWidth, "Beam Width", 0.0f, 20.0f, 0.1f),
			SERIALIZED_FLOAT(m_spiritCannonDamage, "Beam Damage", 0.0f, 100.0f, 1.0f),
			SERIALIZED_FLOAT(m_spiritCannonPhase1ShotInterval, "Phase 1 Shot Interval", 0.0f, 10.0f, 0.05f),
			SERIALIZED_FLOAT(m_spiritCannonPhase2ShotInterval, "Phase 2 Shot Interval", 0.0f, 10.0f, 0.05f),
			SERIALIZED_FLOAT(m_spiritCannonPhase2FinalShotDelay, "Phase 2 Final Shot Delay", 0.0f, 10.0f, 0.05f),
			SERIALIZED_FLOAT(m_spiritCannonPhase2FinalBeamWidth, "Phase 2 Final Beam Width", 0.0f, 20.0f, 0.1f),
			SERIALIZED_FLOAT(m_spiritCannonPhase2FinalDamage, "Phase 2 Final Beam Damage", 0.0f, 200.0f, 1.0f),
			SERIALIZED_FLOAT(m_spiritCannonRecoveryDuration, "Recovery Duration", 0.0f, 10.0f, 0.1f)
		),

		FIELD_GROUP_COLLAPSE("Grasp of the Dead",
			SERIALIZED_FLOAT(m_graspPullDuration, "Pull Duration", 0.0f, 10.0f, 0.1f),
			SERIALIZED_FLOAT(m_graspPullStrength, "Pull Strength", 0.0f, 30.0f, 0.1f)
		),

		FIELD_GROUP_COLLAPSE("Teleport",
			SERIALIZED_FLOAT(m_teleportCastDuration, "Cast Duration", 0.0f, 10.0f, 0.1f),
			SERIALIZED_FLOAT(m_teleportCooldownDuration, "Cooldown Duration", 0.0f, 60.0f, 0.5f),
			SERIALIZED_FLOAT(m_teleportTriggerDistance, "Trigger Distance", 0.0f, 20.0f, 0.1f),
			SERIALIZED_FLOAT(m_teleportPhase2BurstRadius, "Phase 2 Burst Radius", 0.0f, 20.0f, 0.1f),
			SERIALIZED_FLOAT(m_teleportPhase2BurstDamage, "Phase 2 Burst Damage", 0.0f, 9999.0f, 1.0f),
			SERIALIZED_FLOAT(m_teleportRecoveryDuration, "Recovery Duration", 0.0f, 5.0f, 0.05f)
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
#pragma once

#include "EnemyBaseController.h"

class AelorinDetectionAggro;
class AelorinDamageable;
class AelorinAttackConfig;

enum class Phase
{
	Phase1,
	Phase2
};

enum class AelorinAbility
{
	None,

	SeekerSigils,
	Nova,
	RisenSpires,
	SpiritCannon,
	GraspOfTheDead,

	Summon,
	Teleport
};

class AelorinBossController : public EnemyBaseController
{
	DECLARE_SCRIPT(AelorinBossController)

public:
	explicit AelorinBossController(GameObject* owner);

	void Start() override;
	//void drawGizmo() override;
	void Update() override;

	FieldList getExposedFields() const override;

	Vector3 getLyrielPosition() const;
	Vector3 getDeathPosition() const;

	// AttackConfig
	const AelorinAttackConfig* getAelorinAttackConfig() const { return m_attackConfig.get(); }

	// Ability Choosing
	AelorinAbility chooseNextAbility();
	AelorinAbility consumeRequestedAbility();
	bool requestAbility(AelorinAbility ability);

	bool hasRequestedAbility() const { return m_requestedAbility != AelorinAbility::None; }
	AelorinAbility getRequestedAbility() const { return m_requestedAbility; }
	bool trySendRequestedAbilityTrigger(AnimationComponent* animation);
	void clearRequestedAbility();

	// Encounter
	void updateEncounter();
	bool hasEncounterStarted() const { return m_hasStartedEncounter; }

	// Decision Timing helpers
	float getDecisionTime() const;

	// Phase helpers
	Phase getPhase() const { return m_phase; }
	void setPhase(Phase phase);
	bool isPhase2() const { return m_phase == Phase::Phase2; }
	
	void requestPhaseTransition();
	void markPhaseTransitionTriggered();
	void beginPhase2();
	bool isPhaseTransitionRequested() const { return m_phaseTransitionRequested; }
	bool canTriggerPhaseTransition() const { return m_phaseTransitionRequested && !m_phaseTransitionTriggered; }

	bool trySendPhaseTransitionTrigger(AnimationComponent* animation);

	// Threshold Stagger helpers
	void requestThresholdStagger();
	bool trySendThresholdStaggerTrigger(AnimationComponent* animation);
	void completeThresholdStagger();
	float getThresholdStaggerDuration() const;

	// Health drop
	void spawnHealthDrops();

protected:
	Transform* acquireCurrentTarget() override;
	bool isTargetDowned(Transform* target) const override;

public:
	AssetReference<AelorinAttackConfig> m_attackConfig;

private:
	AelorinDetectionAggro* m_aelorinDetectionAggro = nullptr;
	AelorinDamageable* m_damageable = nullptr;

	// Phase
	Phase m_phase = Phase::Phase1;
	bool m_phaseTransitionRequested = false;
	bool m_phaseTransitionTriggered = false;

	// Encounter
	bool m_hasStartedEncounter = false;

	// Threshold Stagger
	bool m_thresholdStaggerRequested = false;
	bool m_thresholdStaggerTriggered = false;

	// Model switching
	GameObject* m_phase1GameObject = nullptr;
	GameObject* m_phase2GameObject = nullptr;

	// Ability
	AelorinAbility m_requestedAbility = AelorinAbility::None;
	AelorinAbility m_lastUsedAbility = AelorinAbility::None;
	bool m_abilityTriggerSent = false;

	// Teleport TODO
	std::vector<Vector3> m_teleportPositions;
	size_t m_currentPositionIndex = 0;

private:
	// Abilities
	std::vector<AelorinAbility> buildAbilityPool() const;
	void removeLastUsedAbility(std::vector<AelorinAbility>& pool) const;

	bool canUseNova() const;
	bool canSummon() const;
	bool canTeleport() const;

	AelorinAbility chooseRandomAbility(const std::vector<AelorinAbility>& pool) const;
};
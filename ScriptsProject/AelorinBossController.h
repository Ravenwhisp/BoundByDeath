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

class AelorinBossController : public EnemyBaseController
{
	DECLARE_SCRIPT(AelorinBossController)

public:
	explicit AelorinBossController(GameObject* owner);

	void Start() override;
	//void drawGizmo() override;
	void Update() override;

	FieldList getExposedFields() const override;

	// AttackConfig
	const AelorinAttackConfig* getAelorinAttackConfig() const { return m_attackConfig.get(); }

	// Encounter
	void updateEncounter();

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

protected:
	Transform* acquireCurrentTarget() override;
	bool isTargetDowned(Transform* target) const override;

public:
	AssetReference<AelorinAttackConfig> m_attackConfig;

private:
	AelorinDetectionAggro* m_aelorinDetectionAggro = nullptr;
	AelorinDamageable* m_damageable = nullptr;

	Phase m_phase = Phase::Phase1;
	bool m_phaseTransitionRequested = false;
	bool m_phaseTransitionTriggered = false;

	bool m_hasStartedEncounter = false;

	bool m_thresholdStaggerRequested = false;
	bool m_thresholdStaggerTriggered = false;

	// Model switching
	GameObject* m_phase1GameObject = nullptr;
	GameObject* m_phase2GameObject = nullptr;

	// Teleport TODO
	std::vector<Vector3> m_teleportPositions;
	size_t m_currentPositionIndex = 0;
};
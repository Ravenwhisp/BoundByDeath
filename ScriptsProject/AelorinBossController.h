#pragma once

#include "EnemyBaseController.h"

class AelorinDetectionAggro;
class AelorinDamageable;

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

	//FieldList getExposedFields() const override;

	// Phase helpers
	void setPhase(Phase phase);
	Phase getPhase() const { return m_phase; }
	bool isPhase2() const { return m_phase == Phase::Phase2; }
	void updateBossPhase();

protected:
	Transform* acquireCurrentTarget() override;
	bool isTargetDowned(Transform* target) const override;

private:
	AelorinDetectionAggro* m_aelorinDetectionAggro = nullptr;
	AelorinDamageable* m_damageable = nullptr;

	Phase m_phase = Phase::Phase1;

	bool m_hasStartedEncounter = false;

	// Model switching
	GameObject* m_phase1GameObject = nullptr;
	GameObject* m_phase2GameObject = nullptr;

	// Teleport TODO
	std::vector<Vector3> m_teleportPositions;
	size_t m_currentPositionIndex = 0;
};
#pragma once

#include "EnemyBaseController.h"

class AelorinDetectionAggro;
class EnemyDamageable;

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
	EnemyDamageable* m_damageable = nullptr;

	Phase m_phase = Phase::Phase1;

	bool m_hasStartedEncounter = false;

	GameObject* m_phase1GameObject = nullptr;
	GameObject* m_phase2GameObject = nullptr;
};
#pragma once

#include "EnemyBaseController.h"
#include "Timer.h"

class EnemyDetectionAggro;
class PaladinAttackConfig;

class MeleeEnemyController : public EnemyBaseController
{
	DECLARE_SCRIPT(MeleeEnemyController)

public:
	explicit MeleeEnemyController(GameObject* owner);

	void Start() override;
	void Update() override;
	FieldList getExposedFields() const override;

	const EnemyBaseAttackConfig* getAttackConfig() const override;

	// Charge helpers
	bool playerInChargeRange() const;

    bool isChargeReady() const;
    void consumeChargeCooldown();

	Vector3 getChargeDirection() const;

protected:
	Transform* acquireCurrentTarget() override;
	bool isTargetDowned(Transform* target) const override;

private:
	EnemyDetectionAggro* m_enemyDetectionAggro = nullptr;

    Timer m_chargeCooldownTimer;

public:
	AssetReference<PaladinAttackConfig> m_attackConfig;
};
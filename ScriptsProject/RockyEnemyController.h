#pragma once

#include "EnemyBaseController.h"

class EnemyDetectionAggro;
class EnemyBaseAttackConfig;

class RockyEnemyController : public EnemyBaseController
{
	DECLARE_SCRIPT(RockyEnemyController)

public:
	explicit RockyEnemyController(GameObject* owner);

	void Start() override;
	void Update() override;
	FieldList getExposedFields() const override;

	const EnemyBaseAttackConfig* getAttackConfig() const override;

protected:
	Transform* acquireCurrentTarget() override;
	bool isTargetDowned(Transform* target) const override;

private:
	EnemyDetectionAggro* m_enemyDetectionAggro = nullptr;

public:
	AssetReference<EnemyBaseAttackConfig> m_attackConfig;
};
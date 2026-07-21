#pragma once

#include "EnemyBaseController.h"
#include "Timer.h"

class EnemyDetectionAggro;
class SummonerAttackConfig;
class Transform;

class SummonerEnemyController : public EnemyBaseController
{
	DECLARE_SCRIPT(SummonerEnemyController)

public:
	explicit SummonerEnemyController(GameObject* owner);

	void Start() override;
	void Update() override;
	FieldList getExposedFields() const override;

	const EnemyBaseAttackConfig* getAttackConfig() const override;

	bool isTeleportReady() const;
	void consumeTeleportCooldown();
	bool tryGetTeleportPosition(Vector3& outPosition) const;

	bool isSummonReady() const;
	void consumeSummonCooldown();
	void summonSpidersAroundSelf();

	float getRecoveryDuration() const;

	bool isAttackReady() const;
	void consumeAttackCooldown();

protected:
	Transform* acquireCurrentTarget() override;
	bool isTargetDowned(Transform* target) const override;

private:
    EnemyDetectionAggro* m_enemyDetectionAggro = nullptr;

    Timer m_attackCooldownTimer;
    Timer m_teleportCooldownTimer;
    Timer m_summonCooldownTimer;

public:
	AssetReference<SummonerAttackConfig> m_attackConfig;
};
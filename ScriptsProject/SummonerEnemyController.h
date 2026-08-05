#pragma once

#include "EnemyBaseController.h"

class EnemyDetectionAggro;
class SummonerAttackConfig;
class Transform;
class SummonerParticles;

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
	void beginSummoningSpiders();

	float getRecoveryDuration() const;

	bool isAttackReady() const;
	void consumeAttackCooldown();

protected:
	Transform* acquireCurrentTarget() override;
	bool isTargetDowned(Transform* target) const override;

private:
	void updateTeleportCooldown(float dt);
	void updateSummonCooldown(float dt);
	void updateAttackCooldown(float dt);

	struct PendingSpiderSummon
	{
		Vector3 position = Vector3::Zero;
		float timer = 0.0f;
	};

	void updatePendingSpiderSummons(float dt);
	void spawnPendingSpider(const Vector3& position);

private:
	EnemyDetectionAggro* m_enemyDetectionAggro = nullptr;
	SummonerParticles* m_particles = nullptr;

	std::vector<PendingSpiderSummon> m_pendingSpiderSummons;

	float m_attackCooldownTimer = 0.0f;
	float m_teleportCooldownTimer = 0.0f;
	float m_summonCooldownTimer = 0.0f;

public:
	AssetReference<SummonerAttackConfig> m_attackConfig;
};
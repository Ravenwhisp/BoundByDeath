#pragma once

#include "EnemyBaseController.h"

#include <vector>

class EnemyDetectionAggro;
class SummonerAttackConfig;
class Transform;

enum class SummonerTeleportMode
{
	None,
	Escape,
	Approach
};

class SummonerEnemyController : public EnemyBaseController
{
	DECLARE_SCRIPT(SummonerEnemyController)

public:
	explicit SummonerEnemyController(GameObject* owner);

	void Start() override;
	void Update() override;
	FieldList getExposedFields() const override;

	const EnemyBaseAttackConfig* getAttackConfig() const override;

	// Teleport
	SummonerTeleportMode getTeleportMode() const;

	bool isTeleportReady() const;
	bool shouldEscapeTeleport() const;
	bool shouldApproachTeleport() const;
	bool tryGetTeleportPosition(SummonerTeleportMode mode, Vector3& outPosition);

	void consumeTeleportCooldown();
	void delayTeleportRetry();

	// Summon
	bool isSummonReady() const;
	void consumeSummonCooldown();
	void summonSpidersAroundSelf();
	void summonSpidersAtPositions(const std::vector<Vector3>& spawnPositions);
	int computeSummonSpawnPositions(std::vector<Vector3>& outPositions) const;

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

	float getClosestActivePlayerDistance(const Vector3& position) const;

private:
	EnemyDetectionAggro* m_enemyDetectionAggro = nullptr;

	float m_attackCooldownTimer = 0.0f;
	float m_teleportCooldownTimer = 0.0f;
	float m_summonCooldownTimer = 0.0f;

public:
	AssetReference<SummonerAttackConfig> m_attackConfig;
};

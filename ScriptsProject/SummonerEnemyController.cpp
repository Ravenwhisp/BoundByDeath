#include "pch.h"
#include "SummonerEnemyController.h"

#include "EnemyDetectionAggro.h"
#include "SummonerAttackConfig.h"

SummonerEnemyController::SummonerEnemyController(GameObject* owner)
	: EnemyBaseController(owner)
{
}

void SummonerEnemyController::Start()
{
	m_enemyDetectionAggro = GameObjectAPI::findScript<EnemyDetectionAggro>(getOwner());
	m_attackConfig = GameObjectAPI::findScript<SummonerAttackConfig>(getOwner());

	if (!m_enemyDetectionAggro)
	{
		Debug::warn("[SummonerEnemyController] EnemyDetectionAggro not found on '%s'.", GameObjectAPI::getName(getOwner()));
	}

	if (!m_attackConfig)
	{
		Debug::warn("[SummonerEnemyController] SummonerAttackConfig not found on '%s'.", GameObjectAPI::getName(getOwner()));
	}

	m_currentTarget = nullptr;
	m_deathTriggerSent = false;

	resetRepathTimer();
	clearPath();
}

void SummonerEnemyController::Update()
{
	const float dt = Time::getDeltaTime();

	updateCurrentTarget();

	updateTeleportCooldown(dt);
	updateSummonCooldown(dt);
	updateAttackCooldown(dt);

	updateStun(dt);
}

Transform* SummonerEnemyController::acquireCurrentTarget()
{
	if (!m_enemyDetectionAggro)
	{
		m_enemyDetectionAggro = GameObjectAPI::findScript<EnemyDetectionAggro>(getOwner());
	}

	if (!m_enemyDetectionAggro)
	{
		return nullptr;
	}

	return m_enemyDetectionAggro->getCurrentTarget();
}

bool SummonerEnemyController::isTargetDowned(Transform* target) const
{
	if (!m_enemyDetectionAggro || !target)
	{
		return true;
	}

	return m_enemyDetectionAggro->isDowned(target);
}

bool SummonerEnemyController::isTargetInAttackRange() const
{
	if (!hasValidTarget() || !m_attackConfig)
	{
		return false;
	}

	return isCurrentTargetInRange(m_attackConfig->m_basicAttackRange);
}

bool SummonerEnemyController::isTeleportReady() const
{
	return m_teleportCooldownTimer <= 0.0f;
}

void SummonerEnemyController::consumeTeleportCooldown()
{
	if (!m_attackConfig)
	{
		return;
	}

	m_teleportCooldownTimer = m_attackConfig->m_teleportCooldown;
}

bool SummonerEnemyController::tryGetTeleportPosition(Vector3& outPosition) const
{
	constexpr int MaxTeleportAttempts = 10;

	if (!m_attackConfig || !hasValidTarget())
	{
		return false;
	}

	Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
	Transform* targetTransform = getCurrentTarget();

	if (!ownerTransform || !targetTransform)
	{
		return false;
	}

	const Vector3 ownerPosition = TransformAPI::getPosition(ownerTransform);
	const Vector3 targetPosition = TransformAPI::getPosition(targetTransform);

	const bool targetInAttackRange = isCurrentTargetInRange(m_attackConfig->m_basicAttackRange);
	const Vector3 searchCenter = targetInAttackRange ? ownerPosition : targetPosition;

	const Vector3 searchExtents = Vector3(5.0f, 5.0f, 5.0f);

	for (int i = 0; i < MaxTeleportAttempts; ++i)
	{
		Vector3 candidatePosition;

		const bool found = NavigationAPI::findRandomReachablePointAround(
			searchCenter,
			m_attackConfig->m_teleportRadius,
			candidatePosition,
			searchExtents,
			1
		);

		if (!found)
		{
			continue;
		}

		Vector3 difference = candidatePosition - targetPosition;
		difference.y = 0.0f;

		const float distanceSquared = difference.LengthSquared();
		const float attackRangeSquared = m_attackConfig->m_basicAttackRange * m_attackConfig->m_basicAttackRange;

		if (distanceSquared <= attackRangeSquared)
		{
			outPosition = candidatePosition;
			return true;
		}
	}

	return false;
}

void SummonerEnemyController::updateTeleportCooldown(float dt)
{
	if (m_teleportCooldownTimer <= 0.0f)
	{
		return;
	}
	
	m_teleportCooldownTimer -= dt;

	if (m_teleportCooldownTimer < 0.0f)
	{
		m_teleportCooldownTimer = 0.0f;
	}
}

bool SummonerEnemyController::isSummonReady() const
{
	return m_summonCooldownTimer <= 0.0f;
}

void SummonerEnemyController::consumeSummonCooldown()
{
	if (!m_attackConfig)
	{
		return;
	}

	m_summonCooldownTimer = m_attackConfig->m_summonCooldown;
}

void SummonerEnemyController::summonSpidersAroundSelf()
{
	if (!m_attackConfig)
	{
		return;
	}

	Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
	if (!ownerTransform)
	{
		return;
	}

	const Vector3 ownerPosition = TransformAPI::getPosition(ownerTransform);
	const Vector3 searchExtents = Vector3(5.0f, 5.0f, 5.0f);

	for (int i = 0; i < m_attackConfig->m_summonCount; ++i)
	{
		Vector3 spawnPosition;

		const bool found = NavigationAPI::findRandomReachablePointAround(
			ownerPosition,
			m_attackConfig->m_summonRadius,
			spawnPosition,
			searchExtents,
			10
		);

		if (!found)
		{
			continue;
		}

		GameObjectAPI::instantiatePrefab(
			m_attackConfig->m_spiderPrefabPath.c_str(),
			spawnPosition,
			Vector3(0.0f, 0.0f, 0.0f)
		);
	}
}

void SummonerEnemyController::updateSummonCooldown(float dt)
{
	if (m_summonCooldownTimer <= 0.0f)
	{
		return;
	}

	m_summonCooldownTimer -= dt;

	if (m_summonCooldownTimer < 0.0f)
	{
		m_summonCooldownTimer = 0.0f;
	}
}

float SummonerEnemyController::getRecoveryDuration() const
{
	if (!m_attackConfig)
	{
		return 0.0f;
	}

	return m_attackConfig->m_summonRecoverDuration;
}

bool SummonerEnemyController::isAttackReady() const
{
	return m_attackCooldownTimer <= 0.0f;
}

void SummonerEnemyController::consumeAttackCooldown()
{
	if (!m_attackConfig)
	{
		return;
	}

	m_attackCooldownTimer = m_attackConfig->m_basicAttackCooldown;
}

void SummonerEnemyController::updateAttackCooldown(float dt)
{
	if (m_attackCooldownTimer <= 0.0f)
	{
		return;
	}

	m_attackCooldownTimer -= dt;

	if (m_attackCooldownTimer < 0.0f)
	{
		m_attackCooldownTimer = 0.0f;
	}
}

IMPLEMENT_SCRIPT(SummonerEnemyController)
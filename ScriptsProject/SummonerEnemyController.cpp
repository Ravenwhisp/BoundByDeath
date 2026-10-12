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
	EnemyBaseController::Start();

	m_enemyDetectionAggro = GameObjectAPI::findScript<EnemyDetectionAggro>(getOwner());
	if (!m_enemyDetectionAggro)
	{
		Debug::warn("[SummonerEnemyController] EnemyDetectionAggro not found on '%s'.", GameObjectAPI::getName(getOwner()));
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

const EnemyBaseAttackConfig* SummonerEnemyController::getAttackConfig() const
{
	return m_attackConfig.get();
}

SummonerTeleportMode SummonerEnemyController::getTeleportMode() const
{
	if (shouldEscapeTeleport())
	{
		return SummonerTeleportMode::Escape;
	}

	if (shouldApproachTeleport())
	{
		return SummonerTeleportMode::Approach;
	}

	return SummonerTeleportMode::None;
}

bool SummonerEnemyController::isTeleportReady() const
{
	return m_teleportCooldownTimer <= 0.0f;
}

bool SummonerEnemyController::shouldEscapeTeleport() const
{
	if (!m_attackConfig || !m_enemyDetectionAggro || !hasValidTarget())
	{
		return false;
	}

	Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
	if (!ownerTransform)
	{
		return false;
	}

	const Vector3 ownerPosition = TransformAPI::getGlobalPosition(ownerTransform);
	const float closestDistance = getClosestActivePlayerDistance(ownerPosition);

	return closestDistance < m_attackConfig.get()->m_teleportEscapeDistance;
}

bool SummonerEnemyController::shouldApproachTeleport() const
{
	if (!m_attackConfig || !hasValidTarget())
	{
		return false;
	}

	return !isCurrentTargetInRange(m_attackConfig.get()->m_basicAttackRange);
}

bool SummonerEnemyController::tryGetTeleportPosition(SummonerTeleportMode mode, Vector3& outPosition)
{
	constexpr int MaxTeleportAttempts = 20;
	constexpr float MinEscapeImprovement = 0.5f;
	constexpr float MinTeleportDisplacement = 1.0f;

	if (!m_attackConfig || !hasValidTarget() || !m_enemyDetectionAggro || mode == SummonerTeleportMode::None)
	{
		return false;
	}

	Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
	Transform* targetTransform = getCurrentTarget();

	if (!ownerTransform || !targetTransform)
	{
		return false;
	}

	const auto* config = m_attackConfig.get();

	const Vector3 ownerPosition = TransformAPI::getGlobalPosition(ownerTransform);
	const Vector3 targetPosition = TransformAPI::getGlobalPosition(targetTransform);
	
	const float currentClosestDistance = getClosestActivePlayerDistance(ownerPosition);
	if (currentClosestDistance == FLT_MAX)
	{
		return false;
	}

	const bool isEscape = mode == SummonerTeleportMode::Escape;

	const float attackRange = config->m_basicAttackRange;
	const float minPlayerDistance = config->m_teleportMinPlayerDistance;

	// Aim for the preferred combat distance
	const float preferredDistance = std::clamp(config->m_teleportPreferredDistance, minPlayerDistance, attackRange);

	// Escape searches around the Summoner / Approach searches around the current target
	const Vector3 searchCenter = isEscape ? ownerPosition : targetPosition;

	const Vector3 searchExtents(5.0f, 5.0f, 5.0f);

	float bestScore = -FLT_MAX;
	bool foundPosition = false;
	Vector3 bestPosition;

	for (int i = 0; i < MaxTeleportAttempts; ++i)
	{
		Vector3 candidatePosition;
		
		const bool found = NavigationAPI::findRandomReachablePointAround(
			searchCenter,
			config->m_teleportRadius,
			candidatePosition,
			searchExtents,
			1
		);

		if (!found)
		{
			continue;
		}

		// Avoid teleporting almost to the same location
		Vector3 displacement = candidatePosition - ownerPosition;
		displacement.y = 0.0f;

		if (displacement.LengthSquared() < MinTeleportDisplacement * MinTeleportDisplacement)
		{
			continue;
		}

		// Check distance to every active player
		const float closestPlayerDistance = getClosestActivePlayerDistance(candidatePosition);
		if (closestPlayerDistance == FLT_MAX)
		{
			continue;
		}

		// Don't select position too close to a player
		if (closestPlayerDistance < minPlayerDistance)
		{
			continue;
		}

		Vector3 toTarget = candidatePosition - targetPosition;
		toTarget.y = 0.0f;

		const float targetDistance = toTarget.Length();
		float score = 0.0f;

		if (isEscape)
		{
			// New position must improve safety
			if (closestPlayerDistance < currentClosestDistance + MinEscapeImprovement)
			{
				continue;
			}

			const float desiredEscapeDistance =	(std::max)(config->m_teleportPreferredDistance,	config->m_teleportEscapeDistance);

			score = -std::abs(closestPlayerDistance - desiredEscapeDistance);
		}
		else
		{
			// Approach must place it inside projectile range
			if (targetDistance > attackRange)
			{
				continue;
			}

			// Prefer the desired ranged combat distance
			score = -std::abs(targetDistance - preferredDistance);

			// Small preference for keeping other player away
			score += closestPlayerDistance * 0.01f;
		}

		if (score > bestScore)
		{
			bestScore = score;
			bestPosition = candidatePosition;
			foundPosition = true;
		}
	}

	if (!foundPosition)
	{
		return false;
	}

	outPosition = bestPosition;
	return true;
}

void SummonerEnemyController::consumeTeleportCooldown()
{
	if (!m_attackConfig)
	{
		return;
	}

	m_teleportCooldownTimer = m_attackConfig.get()->m_teleportCooldown;
}

void SummonerEnemyController::delayTeleportRetry()
{
	if (!m_attackConfig)
	{
		return;
	}

	m_teleportCooldownTimer = m_attackConfig.get()->m_teleportRetryDelay;
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

	m_summonCooldownTimer = m_attackConfig.get()->m_summonCooldown;
}

void SummonerEnemyController::summonSpidersAroundSelf()
{
	std::vector<Vector3> spawnPositions;
	computeSummonSpawnPositions(spawnPositions);
	summonSpidersAtPositions(spawnPositions);
}

void SummonerEnemyController::summonSpidersAtPositions(const std::vector<Vector3>& spawnPositions)
{
	if (!m_attackConfig)
	{
		return;
	}

	for (const Vector3& spawnPosition : spawnPositions)
	{
		GameObjectAPI::instantiatePrefab(
			m_attackConfig.get()->m_spiderPrefab.m_id,
			spawnPosition,
			Vector3(0.0f, 0.0f, 0.0f)
		);
	}
}

int SummonerEnemyController::computeSummonSpawnPositions(std::vector<Vector3>& outPositions) const
{
	outPositions.clear();

	if (!m_attackConfig)
	{
		return 0;
	}

	Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
	if (!ownerTransform)
	{
		return 0;
	}

	const Vector3 ownerPosition = TransformAPI::getGlobalPosition(ownerTransform);
	const Vector3 searchExtents = Vector3(5.0f, 5.0f, 5.0f);

	for (int i = 0; i < m_attackConfig.get()->m_summonCount; ++i)
	{
		Vector3 spawnPosition;

		const bool found = NavigationAPI::findRandomReachablePointAround(
			ownerPosition,
			m_attackConfig.get()->m_summonRadius,
			spawnPosition,
			searchExtents,
			10
		);

		if (!found)
		{
			continue;
		}

		outPositions.push_back(spawnPosition);
	}

	return static_cast<int>(outPositions.size());
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

	return m_attackConfig.get()->m_summonRecoverDuration;
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

	m_attackCooldownTimer = m_attackConfig.get()->m_basicAttackCooldown;
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

float SummonerEnemyController::getClosestActivePlayerDistance(const Vector3& position) const
{
	if (!m_enemyDetectionAggro)
	{
		return FLT_MAX;
	}

	float closestDistance = FLT_MAX;

	Transform* players[] =
	{
		m_enemyDetectionAggro->getLyrielTransform(),
		m_enemyDetectionAggro->getDeathTransform()
	};

	for (Transform* player : players)
	{
		if (!player || m_enemyDetectionAggro->isDowned(player))
		{
			continue;
		}

		Vector3 difference = TransformAPI::getGlobalPosition(player) - position;
		difference.y = 0.0f;

		const float distance = difference.Length();
		if (distance < closestDistance)
		{
			closestDistance = distance;
		}
	}

	return closestDistance;
}

IMPLEMENT_SCRIPT_FIELDS_INHERITED(SummonerEnemyController, EnemyBaseController,
    SERIALIZED_ASSET_REF(m_attackConfig, "Attack Config", AssetType::DATA_CONTAINER)
)

IMPLEMENT_SCRIPT(SummonerEnemyController)

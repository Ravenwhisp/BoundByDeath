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

IMPLEMENT_SCRIPT(SummonerEnemyController)
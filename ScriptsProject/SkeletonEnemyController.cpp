#include "pch.h"
#include "SkeletonEnemyController.h"

#include "EnemyDetectionAggro.h"
#include "SkeletonAttackConfig.h"

SkeletonEnemyController::SkeletonEnemyController(GameObject* owner)
	: EnemyBaseController(owner)
{
}

void SkeletonEnemyController::Start()
{
	m_enemyDetectionAggro = GameObjectAPI::findScript<EnemyDetectionAggro>(getOwner());
	m_attackConfig = GameObjectAPI::findScript<SkeletonAttackConfig>(getOwner());

	if (!m_enemyDetectionAggro)
	{
		Debug::warn("[SkeletonEnemyController] EnemyDetectionAggro not found on '%s'.", GameObjectAPI::getName(getOwner()));
	}

	if (!m_attackConfig)
	{
		Debug::warn("[SkeletonEnemyController] SkeletonAttackConfig not found on '%s'.", GameObjectAPI::getName(getOwner()));
	}

	m_currentTarget = nullptr;
	m_deathTriggerSent = false;

	resetRepathTimer();
	clearPath();
}

void SkeletonEnemyController::Update()
{
	const float dt = Time::getDeltaTime();

	updateCurrentTarget();
	updateGuardCooldown(dt);
	//updateStun(dt);
}

Transform* SkeletonEnemyController::acquireCurrentTarget()
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

bool SkeletonEnemyController::isTargetDowned(Transform* target) const
{
	if (!m_enemyDetectionAggro || !target)
	{
		return true;
	}

	return m_enemyDetectionAggro->isDowned(target);
}

bool SkeletonEnemyController::isTargetInScimitarRange() const
{
	if (!hasValidTarget() || !m_attackConfig)
	{
		return false;
	}

	return isCurrentTargetInRange(m_attackConfig->m_basicAttackRange);
}

bool SkeletonEnemyController::isGuardReady() const
{
	return m_guardCooldownTimer <= 0.0f;
}

void SkeletonEnemyController::consumeGuardCooldown()
{
	if (!m_attackConfig)
	{
		return;
	}

	m_guardCooldownTimer = m_attackConfig->m_guardCooldown;
}

void SkeletonEnemyController::updateGuardCooldown(float dt)
{
	if (m_guardCooldownTimer <= 0.0f)
	{
		return;
	}

	m_guardCooldownTimer -= dt;

	if (m_guardCooldownTimer < 0.0f)
	{
		m_guardCooldownTimer = 0.0f;
	}
}

IMPLEMENT_SCRIPT(SkeletonEnemyController)
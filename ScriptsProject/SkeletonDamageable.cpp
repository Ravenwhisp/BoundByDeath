#include "pch.h"
#include "SkeletonDamageable.h"

#include "SkeletonEnemyController.h"
#include "SkeletonAttackConfig.h"
#include <cmath>

SkeletonDamageable::SkeletonDamageable(GameObject* owner)
	: EnemyDamageable(owner)
{
}

void SkeletonDamageable::Start()
{
	EnemyDamageable::Start();

	m_skeletonController = GameObjectAPI::findScript<SkeletonEnemyController>(getOwner());
	m_attackConfig = GameObjectAPI::findScript<SkeletonAttackConfig>(getOwner());

	if (!m_skeletonController)
	{
		Debug::warn("[SkeletonDamageable] SkeletonEnemyController not found.");
	}

	if (!m_attackConfig)
	{
		Debug::warn("[SkeletonDamageable] SkeletonAttackConfig not found.");
	}
}

void SkeletonDamageable::takeDamage(const HitContext& ctx)
{
	const EnemyHitContext& enemyCtx = static_cast<const EnemyHitContext&>(ctx);

	if (isDowned() && m_currentHp <= 0.0f)
	{
		confirmKill();
		return;
	}

	if (shouldBlockDamage(enemyCtx))
	{
		Debug::log("[SkeletonDamageable] Damage blocked by Guard.");
		return;
	}

	EnemyDamageable::takeDamage(ctx);
}

void SkeletonDamageable::onHpDepleted()
{
	if (m_lifeState == SkeletonLifeState::Alive)
	{
		startDowned();
		return;
	}

	confirmKill();
}

bool SkeletonDamageable::isDowned() const
{
	return m_lifeState == SkeletonLifeState::Downed;
}

bool SkeletonDamageable::isPermanentlyDead() const
{
	return m_lifeState == SkeletonLifeState::PermanentlyDead;
}

void SkeletonDamageable::startDowned()
{
	m_lifeState = SkeletonLifeState::Downed;
	m_previousMaxHp = getMaxHp();
	m_maxHp = m_attackConfig->m_downedHP;
	m_currentHp = m_attackConfig->m_downedHP;
	m_isDead = false;

	Debug::log("[SkeletonDamageable] Skeleton downed.");
}

void SkeletonDamageable::confirmKill()
{
	m_lifeState = SkeletonLifeState::PermanentlyDead;
}

void SkeletonDamageable::completeRevive()
{
	if (m_lifeState != SkeletonLifeState::Downed)
	{
		return;
	}

	m_maxHp = m_previousMaxHp;
	m_lifeState = SkeletonLifeState::Alive;
	revive(m_previousMaxHp * 0.5f);

	Debug::log("[SkeletonDamageable] Skeleton revived at 50% HP.");
}

bool SkeletonDamageable::shouldBlockDamage(const EnemyHitContext& enemyCtx) const
{
	if (!m_skeletonController)
	{
		return false;
	}

	if (!m_attackConfig)
	{
		return false;
	}

	if (!m_skeletonController->isGuarding())
	{
		return false;
	}

	Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
	if (!ownerTransform)
	{
		return false;
	}

	Vector3 ownerPosition = TransformAPI::getGlobalPosition(ownerTransform);

	if (!enemyCtx.attacker)
	{
		return false;
	}

	Vector3 attackerPosition = TransformAPI::getGlobalPosition(enemyCtx.attacker);

	Vector3 toAttacker = attackerPosition - ownerPosition;
	toAttacker.y = 0.0f;

	if (toAttacker.LengthSquared() <= 0.0001f)
	{
		return true;
	}

	Vector3 forward = TransformAPI::getForward(ownerTransform);
	forward.y = 0.0f;

	if (forward.LengthSquared() <= 0.0001f)
	{
		return false;
	}

	toAttacker.Normalize();
	forward.Normalize();

	const float dot = forward.Dot(toAttacker);

	constexpr float degreesToRadians = 3.14159265f / 180.0f;
	const float minDot = std::cos(m_attackConfig->m_guardBlockHalfAngleDegrees * degreesToRadians);

	return dot >= minDot; // if dot >= minDot - attacker is in front | if dot < minDot - attacker is side/back
}

IMPLEMENT_SCRIPT(SkeletonDamageable)
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

	if (shouldBlockDamage(enemyCtx))
	{
		Debug::log("[SkeletonDamageable] Damage blocked by Guard.");
		return;
	}

	EnemyDamageable::takeDamage(ctx);
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
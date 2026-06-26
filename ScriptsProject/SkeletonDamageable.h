#pragma once

#include "EnemyDamageable.h"

class SkeletonEnemyController;
class SkeletonAttackConfig;

class SkeletonDamageable final : public EnemyDamageable
{
	DECLARE_SCRIPT(SkeletonDamageable)

public:
	explicit SkeletonDamageable(GameObject* owner);

	void Start() override;
	void takeDamage(const HitContext& ctx) override;

private:
	bool shouldBlockDamage(const EnemyHitContext& enemyCtx) const;

private:
	SkeletonEnemyController* m_skeletonController = nullptr;
	SkeletonAttackConfig* m_attackConfig = nullptr;
};
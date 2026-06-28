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
	
	bool isDowned() const;
	bool isPermanentlyDead() const;

	void completeRevive();

protected:
	void onHpDepleted() override;

private:
	enum class SkeletonLifeState
	{
		Alive,
		Downed,
		PermanentlyDead
	};

	bool shouldBlockDamage(const EnemyHitContext& enemyCtx) const;
	void startDowned();
	void confirmKill();

private:
	SkeletonEnemyController* m_skeletonController = nullptr;
	SkeletonAttackConfig* m_attackConfig = nullptr;
	SkeletonLifeState m_lifeState = SkeletonLifeState::Alive;
};
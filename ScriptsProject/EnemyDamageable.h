#pragma once

#include "Damageable.h"
#include "EnemyAttackType.h"

class EnemyDetectionAggro;
class EnemySound;

struct EnemyHitContext : public HitContext
{
	Transform* attacker = nullptr;
	EnemyAttackType attackType = EnemyAttackType::None;
};

class EnemyDamageable : public Damageable
{
	DECLARE_SCRIPT(EnemyDamageable)

	public:
		explicit EnemyDamageable(GameObject* owner);

		void Start() override;
		void takeDamage(const HitContext& ctx) override;

	protected:
		void onDamaged(float amount) override;

private:
	EnemyDetectionAggro* m_enemyDetectionAggro = nullptr;
	EnemySound* m_enemySound = nullptr;
	Transform* m_damageSource = nullptr;
};

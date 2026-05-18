#pragma once

#include "Damageable.h"

class EnemyDetectionAggro;

enum class EnemyAttackType
{
	None = 0,
	DeathBasic,
	DeathCharged,
	DeathDash,
	LyrielArrow,
	LyrielVolley,
	LyrielCharged,
	ShadowExecution,
	Environment
};

struct EnemyHitContext
{
	float damage = 0.0f;
	Transform* attacker = nullptr;
	EnemyAttackType attackType = EnemyAttackType::None;
};

class EnemyDamageable : public Damageable
{
	DECLARE_SCRIPT(EnemyDamageable)

	public:
		explicit EnemyDamageable(GameObject* owner);

		void Start() override;
		void takeDamageEnemy(float amount, Transform* playerTransform, EnemyAttackType attackType = EnemyAttackType::None);
		virtual void applyHit(const EnemyHitContext& hit);

	protected:
		void onDamaged(float amount) override;

private:
	EnemyDetectionAggro* m_enemyDetectionAggro = nullptr;
	Transform* m_damageSource = nullptr;
};

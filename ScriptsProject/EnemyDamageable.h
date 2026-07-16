#pragma once

#include "Damageable.h"
#include "EnemyAttackType.h"

class EnemyDetectionAggro;
class EnemySound;
class EnemyShadowMark;

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

		bool lastHitExploitShadowMark() const { return m_lastHitExploitedShadowMark; }
	protected:
		void onDamaged(float amount) override;

private:
	EnemyDetectionAggro* m_enemyDetectionAggro = nullptr;
	EnemySound* m_enemySound = nullptr;
	EnemyShadowMark* m_shadowMark = nullptr;
	Transform* m_damageSource = nullptr;

	bool m_lastHitExploitedShadowMark = false;
};

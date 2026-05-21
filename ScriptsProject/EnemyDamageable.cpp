#include "pch.h"
#include "EnemyDamageable.h"

#include "EnemyDetectionAggro.h"

EnemyDamageable::EnemyDamageable(GameObject* owner)
	: Damageable(owner)
{
}

void EnemyDamageable::Start()
{
	Damageable::Start();

	m_enemyDetectionAggro = GameObjectAPI::findScript<EnemyDetectionAggro>(m_owner);

	if (!m_enemyDetectionAggro)
	{
		Debug::warn("EnemyDetectionAggro Script is missing from %s", GameObjectAPI::getName(m_owner));
	}

}

void EnemyDamageable::onDamaged(float amount)
{
	Damageable::onDamaged(amount);
	
	if (!m_enemyDetectionAggro)
	{
		return;
	}

	if (!m_damageSource)
	{
		return;
	}

	m_enemyDetectionAggro->notifyPlayerAttackedEnemy(m_damageSource);
}

void EnemyDamageable::takeDamageEnemy(const EnemyHitContext& hit)
{
	if (hit.attacker)
	{
		m_damageSource = hit.attacker;
	}

	Damageable::takeDamage(hit.damage);

	m_damageSource = nullptr;
}

void EnemyDamageable::takeDamageEnemy(float amount, Transform* playerTransform, EnemyAttackType attackType)
{
	EnemyHitContext hit;
	hit.damage = amount;
	hit.attacker = playerTransform;
	hit.attackType = attackType;

	takeDamageEnemy(hit);
}

IMPLEMENT_SCRIPT(EnemyDamageable)

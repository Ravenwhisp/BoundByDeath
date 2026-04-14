#include "pch.h"
#include "EnemyATTACK.h"
#include "EnemyController.h"

static const ScriptFieldInfo ATTACKFields[] =
{
	{ "Attack Damage", ScriptFieldType::Float, offsetof(EnemyATTACK, m_attackDamage), { 0.0f, 999999.0f, 1.0f } },
	{ "Attack Cooldown", ScriptFieldType::Float, offsetof(EnemyATTACK, m_attackCooldown), { 0.0f, 10.0f, 0.1f } },
	{ "Debug Enabled", ScriptFieldType::Bool, offsetof(EnemyATTACK, m_debugEnabled) }
};

IMPLEMENT_SCRIPT_FIELDS(EnemyATTACK, ATTACKFields)

EnemyATTACK::EnemyATTACK(GameObject* owner) : StateMachineScript(owner)
{
}

void EnemyATTACK::OnStateEnter()
{
	Script* script = GameObjectAPI::getScript(getOwner(), "EnemyController");
	m_enemyController = dynamic_cast<EnemyController*>(script);

	m_attackTimer = m_attackCooldown;

	if (m_debugEnabled)
	{
		Debug::log("[EnemyATTACK] ENTER");
	}
}

void EnemyATTACK::OnStateUpdate()
{
	if (!m_enemyController)
	{
		return;
	}

	AnimationComponent* animation = AnimationAPI::getAnimationComponent(getOwner());
	if (!animation)
	{
		return;
	}

	m_enemyController->updateCurrentTarget();

	if (!m_enemyController->hasValidTarget())
	{
		AnimationAPI::playState(animation, "Idle");
		return;
	}

	if (!m_enemyController->isTargetInCombatRange())
	{
		AnimationAPI::playState(animation, "Chase");
		return;
	}

	m_enemyController->faceCurrentTarget();

	m_attackTimer += Time::getDeltaTime();

	if (m_attackTimer >= m_attackCooldown)
	{
		performAttack();
		m_attackTimer = 0.0f;
	}
}

void EnemyATTACK::OnStateExit()
{
	if (m_debugEnabled)
	{
		Debug::log("[EnemyATTACK] EXIT");
	}
}

void EnemyATTACK::performAttack()
{
	if (m_debugEnabled)
	{
		Debug::log("[EnemyATTACK] Attack triggered. Damage: %.2f", m_attackDamage);
	}
}

IMPLEMENT_SCRIPT(EnemyATTACK)
#include "pch.h"
#include "EnemyATTACK.h"
#include "EnemyController.h"
#include "Damageable.h"

static Damageable* findDamageable(GameObject* gameObject)
{
	if (!gameObject)
	{
		return nullptr;
	}

	Script* script = GameObjectAPI::getScript(gameObject, "PlayerDamageable");
	Damageable* damageable = dynamic_cast<Damageable*>(script);

	if (damageable)
	{
		return damageable;
	}

	script = GameObjectAPI::getScript(gameObject, "Damageable");
	damageable = dynamic_cast<Damageable*>(script);

	return damageable;
}

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
		AnimationAPI::playState(animation, "Attack");
		return;
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
	if (!m_enemyController)
	{
		return;
	}

	Transform* targetTransform = m_enemyController->getCurrentTarget();
	if (!targetTransform)
	{
		if (m_debugEnabled)
		{
			Debug::warn("[EnemyATTACK] No current target.");
		}
		return;
	}

	GameObject* targetObject = ComponentAPI::getOwner(targetTransform);
	if (!targetObject)
	{
		if (m_debugEnabled)
		{
			Debug::warn("[EnemyATTACK] Could not resolve target GameObject.");
		}
		return;
	}

	Damageable* damageable = findDamageable(targetObject);
	if (!damageable)
	{
		if (m_debugEnabled)
		{
			Debug::warn("[EnemyATTACK] No PlayerDamageable or Damageable found on '%s'.", GameObjectAPI::getName(targetObject));
		}
		return;
	}

	damageable->takeDamage(m_attackDamage);

	if (m_debugEnabled)
	{
		Debug::log("[EnemyATTACK] Attack triggered on '%s'. Damage: %.2f", GameObjectAPI::getName(targetObject), m_attackDamage);
	}
}

IMPLEMENT_SCRIPT(EnemyATTACK)
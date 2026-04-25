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
	{ "Attack Total Duration", ScriptFieldType::Float, offsetof(EnemyATTACK, m_attackTotalDuration), { 0.1f, 5.0f, 0.05f } },
	{ "Damage Trigger Time", ScriptFieldType::Float, offsetof(EnemyATTACK, m_damageTriggerTime), { 0.0f, 5.0f, 0.05f } },
	{ "Attack Commit Duration", ScriptFieldType::Float, offsetof(EnemyATTACK, m_attackCommitDuration), { 0.0f, 2.0f, 0.05f } },
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

	m_attackTimer = 0.0f;
	m_attackCommitTimer = 0.0f;
	m_stateTimer = 0.0f;
	m_hasAppliedDamage = false;

	if (m_enemyController)
	{
		m_enemyController->clearPath();
		m_enemyController->resetRepathTimer();
		m_enemyController->updateCurrentTarget();
		m_enemyController->faceCurrentTarget();
	}

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

	float dt = Time::getDeltaTime();
	m_stateTimer += dt;

	m_enemyController->updateCurrentTarget();

	if (m_enemyController->hasValidTarget())
	{
		m_enemyController->faceCurrentTarget();
	}

	if (!m_hasAppliedDamage && m_stateTimer >= m_damageTriggerTime)
	{
		if (m_enemyController->hasValidTarget() && m_enemyController->isTargetInAttackExitRange())
		{
			performAttack();
		}

		m_hasAppliedDamage = true;
	}

	if (m_stateTimer < m_attackTotalDuration)
	{
		return;
	}

	if (!m_enemyController->hasValidTarget())
	{
		AnimationAPI::playState(animation, "Idle");
		return;
	}

	AnimationAPI::playState(animation, "Recover");
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
	if (damageable->isDead())
	{
		if (m_debugEnabled)
		{
			Debug::log("[EnemyATTACK] Target '%s' is already dead. Attack skipped.", GameObjectAPI::getName(targetObject));
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
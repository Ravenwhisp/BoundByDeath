#include "pch.h"
#include "EnemyCHASE.h"
#include "EnemyController.h"

static const ScriptFieldInfo CHASEFields[] =
{
	{ "Use Charge", ScriptFieldType::Bool, offsetof(EnemyCHASE, m_useCharge) },
	{ "Charge Trigger Range", ScriptFieldType::Float, offsetof(EnemyCHASE, m_chargeTriggerRange), { 0.0f, 50.0f, 0.1f } },
	{ "Debug Enabled", ScriptFieldType::Bool, offsetof(EnemyCHASE, m_debugEnabled) }
};

IMPLEMENT_SCRIPT_FIELDS(EnemyCHASE, CHASEFields)

EnemyCHASE::EnemyCHASE(GameObject* owner) : StateMachineScript(owner)
{
}

void EnemyCHASE::OnStateEnter()
{
	Script* script = GameObjectAPI::getScript(getOwner(), "EnemyController");
	m_enemyController = dynamic_cast<EnemyController*>(script);

	if (!m_enemyController)
	{
		return;
	}

	m_enemyController->clearPath();
	m_enemyController->resetRepathTimer();

	if (m_debugEnabled)
	{
		Debug::log("[EnemyCHASE] ENTER");
	}

	m_enemyController->updateCurrentTarget();

	if (m_enemyController->hasValidTarget())
	{
		m_enemyController->buildPathToTarget();
	}
}

void EnemyCHASE::OnStateUpdate()
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

	m_enemyController->tickChargeCooldown(Time::getDeltaTime());
	m_enemyController->updateCurrentTarget();

	if (!m_enemyController->hasValidTarget())
	{
		AnimationAPI::playState(animation, "Idle"); // sendTrigger
		return;
	}

	Transform* currentTarget = m_enemyController->getCurrentTarget();

	if (m_useCharge && currentTarget && m_enemyController->isChargeReady())
	{
		Vector3 ownerPosition = getOwner()->GetTransform()->getPosition();
		Vector3 targetPosition = currentTarget->getPosition();

		Vector3 difference = targetPosition - ownerPosition;
		difference.y = 0.0f;

		float distanceToTarget = difference.Length();

		if (distanceToTarget <= m_chargeTriggerRange && distanceToTarget > m_enemyController->m_combatRange)
		{
			m_enemyController->faceCurrentTarget();
			m_enemyController->consumeChargeCooldown();
			AnimationAPI::playState(animation, "Charge");
			return;
		}
	}

	if (m_enemyController->isTargetInCombatRange())
	{
		m_enemyController->faceCurrentTarget();
		AnimationAPI::playState(animation, "Attack");
		return;
	}

	m_enemyController->addToRepathTimer(Time::getDeltaTime());

	if (m_enemyController->shouldRepath())
	{
		m_enemyController->buildPathToTarget();
		m_enemyController->resetRepathTimer();
	}

	m_enemyController->followPath();
}

void EnemyCHASE::OnStateExit()
{
	if (!m_debugEnabled)
	{
		return;
	}

	Debug::log("[EnemyCHASE] EXIT");

	if (!m_enemyController)
	{
		return;
	}

	m_enemyController->clearPath();
	m_enemyController->resetRepathTimer();
}

IMPLEMENT_SCRIPT(EnemyCHASE)
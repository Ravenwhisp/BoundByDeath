#include "pch.h"
#include "EnemyDAMAGETAKEN.h"
#include "EnemyController.h"

static const ScriptFieldInfo DAMAGETAKENFields[] =
{
	{ "Duration", ScriptFieldType::Float, offsetof(EnemyDAMAGETAKEN, m_duration), { 0.0f, 5.0f, 0.05f } },
	{ "Debug Enabled", ScriptFieldType::Bool, offsetof(EnemyDAMAGETAKEN, m_debugEnabled) }
};

IMPLEMENT_SCRIPT_FIELDS(EnemyDAMAGETAKEN, DAMAGETAKENFields)

EnemyDAMAGETAKEN::EnemyDAMAGETAKEN(GameObject* owner)
	: StateMachineScript(owner)
{
}

void EnemyDAMAGETAKEN::OnStateEnter()
{
	m_elapsedTime = 0.0f;

	Script* script = GameObjectAPI::getScript(getOwner(), "EnemyController");
	m_enemyController = dynamic_cast<EnemyController*>(script);

	if (m_enemyController)
	{
		m_enemyController->clearPath();
		m_enemyController->resetRepathTimer();
		m_enemyController->updateCurrentTarget();
		m_enemyController->faceCurrentTarget();
	}

	if (m_debugEnabled)
	{
		Debug::log("[EnemyDAMAGETAKEN] ENTER");
	}
}

void EnemyDAMAGETAKEN::OnStateUpdate()
{
	AnimationComponent* animation = AnimationAPI::getAnimationComponent(getOwner());
	if (!animation)
	{
		return;
	}

	m_elapsedTime += Time::getDeltaTime();

	if (m_elapsedTime < m_duration)
	{
		return;
	}

	if (!m_enemyController)
	{
		AnimationAPI::playState(animation, "Idle");
		return;
	}

	m_enemyController->updateCurrentTarget();

	if (!m_enemyController->hasValidTarget())
	{
		AnimationAPI::playState(animation, "Idle");
		return;
	}

	if (m_enemyController->isTargetInAttackEnterRange())
	{
		m_enemyController->faceCurrentTarget();
		AnimationAPI::playState(animation, "Attack");
		return;
	}

	AnimationAPI::playState(animation, "Chase");
}

void EnemyDAMAGETAKEN::OnStateExit()
{
	if (m_debugEnabled)
	{
		Debug::log("[EnemyDAMAGETAKEN] EXIT");
	}
}

IMPLEMENT_SCRIPT(EnemyDAMAGETAKEN)
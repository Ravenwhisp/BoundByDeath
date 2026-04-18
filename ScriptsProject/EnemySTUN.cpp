#include "pch.h"
#include "EnemySTUN.h"
#include "EnemyController.h"

static const ScriptFieldInfo STUNFields[] =
{
	{ "Stun Duration", ScriptFieldType::Float, offsetof(EnemySTUN, m_stunDuration), { 0.0f, 10.0f, 0.1f } },
	{ "Debug Enabled", ScriptFieldType::Bool, offsetof(EnemySTUN, m_debugEnabled) }
};

IMPLEMENT_SCRIPT_FIELDS(EnemySTUN, STUNFields)

EnemySTUN::EnemySTUN(GameObject* owner)
	: StateMachineScript(owner)
{
}

void EnemySTUN::OnStateEnter()
{
	m_elapsedTime = 0.0f;

	Script* script = GameObjectAPI::getScript(getOwner(), "EnemyController");
	m_enemyController = dynamic_cast<EnemyController*>(script);

	if (m_enemyController)
	{
		m_enemyController->clearPath();
		m_enemyController->resetRepathTimer();
	}

	if (m_debugEnabled)
	{
		Debug::log("[EnemySTUN] ENTER");
	}
}

void EnemySTUN::OnStateUpdate()
{
	AnimationComponent* animation = AnimationAPI::getAnimationComponent(getOwner());
	if (!animation)
	{
		return;
	}

	m_elapsedTime += Time::getDeltaTime();

	if (m_elapsedTime < m_stunDuration)
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

	if (m_enemyController->isTargetInCombatRange())
	{
		m_enemyController->faceCurrentTarget();
		AnimationAPI::playState(animation, "Attack");
		return;
	}

	AnimationAPI::playState(animation, "Chase");
}

void EnemySTUN::OnStateExit()
{
	if (m_debugEnabled)
	{
		Debug::log("[EnemySTUN] EXIT");
	}
}

IMPLEMENT_SCRIPT(EnemySTUN)
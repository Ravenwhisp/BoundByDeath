#include "pch.h"
#include "EnemyIDLE.h"
#include "Enemy_Controller.h"

IMPLEMENT_SCRIPT_FIELDS(EnemyIDLE,
	SERIALIZED_BOOL(m_debugEnabled, "Debug Enabled")
)

EnemyIDLE::EnemyIDLE(GameObject* owner) : StateMachineScript(owner)
{
}

void EnemyIDLE::OnStateEnter()
{
	Script* script = GameObjectAPI::getScript(getOwner(), "EnemyController");
	m_enemyController = dynamic_cast<Enemy_Controller*>(script);

	if (!m_enemyController)
	{
		return;
	}

	m_enemyController->clearPath();
	m_enemyController->resetRepathTimer();

	if (m_debugEnabled)
	{
		Debug::log("[EnemyIDLE] ENTER");
	}
}

void EnemyIDLE::OnStateUpdate()
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

void EnemyIDLE::OnStateExit()
{
	if (!m_debugEnabled)
	{
		return;
	}

	Debug::log("[EnemyIDLE] EXIT");
}

IMPLEMENT_SCRIPT(EnemyIDLE)
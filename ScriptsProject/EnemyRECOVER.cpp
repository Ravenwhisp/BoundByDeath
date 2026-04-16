#include "pch.h"
#include "EnemyRECOVER.h"
#include "EnemyController.h"

static const ScriptFieldInfo RECOVERFields[] =
{
	{ "Recover Duration", ScriptFieldType::Float, offsetof(EnemyRECOVER, m_recoverDuration), { 0.0f, 10.0f, 0.1f } },
	{ "Debug Enabled", ScriptFieldType::Bool, offsetof(EnemyRECOVER, m_debugEnabled) }
};

IMPLEMENT_SCRIPT_FIELDS(EnemyRECOVER, RECOVERFields)

EnemyRECOVER::EnemyRECOVER(GameObject* owner) : StateMachineScript(owner)
{
}

void EnemyRECOVER::OnStateEnter()
{
	Script* script = GameObjectAPI::getScript(getOwner(), "EnemyController");
	m_enemyController = dynamic_cast<EnemyController*>(script);

	m_recoverTimer = 0.0f;

	if (m_debugEnabled)
	{
		Debug::log("[EnemyRECOVER] ENTER");
	}
}

void EnemyRECOVER::OnStateUpdate()
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
		AnimationAPI::playState(animation, "Paladin_Idle");
		return;
	}

	if (!m_enemyController->isTargetInCombatRange())
	{
		AnimationAPI::playState(animation, "Paladin_Walk");
		return;
	}

	m_enemyController->faceCurrentTarget();

	m_recoverTimer += Time::getDeltaTime();

	if (m_recoverTimer >= m_recoverDuration)
	{
		AnimationAPI::playState(animation, "Paladin_Attack");
		return;
	}
}

void EnemyRECOVER::OnStateExit()
{
	if (m_debugEnabled)
	{
		Debug::log("[EnemyRECOVER] EXIT");
	}
}

IMPLEMENT_SCRIPT(EnemyRECOVER)
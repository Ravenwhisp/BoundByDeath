#include "pch.h"
#include "RockyChaseState.h"

#include "RockyEnemyController.h"

RockyChaseState::RockyChaseState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void RockyChaseState::OnStateEnter()
{
	m_rockyController = GameObjectAPI::findScript<RockyEnemyController>(getOwner());
	m_animation = AnimationAPI::getAnimationComponent(getOwner());

	if (!m_rockyController)
	{
		Debug::error("[RockyChaseState] RockyEnemyController not found.");
		return;
	}

	if (!m_animation)
	{
		Debug::error("[RockyChaseState] AnimationComponent not found.");
		return;
	}

	m_rockyController->clearPath();
	m_rockyController->resetRepathTimer();

	Debug::log("[RockyChaseState] ENTER");
}

void RockyChaseState::OnStateUpdate()
{
	if (!m_rockyController || !m_animation)
	{
		return;
	}

	if (m_rockyController->trySendDeathTrigger(m_animation))
	{
		return;
	}

	if (!m_rockyController->hasValidTarget())
	{
		AnimationAPI::sendTrigger(m_animation, "ToIdle");
		Debug::log("[RockyChaseState] Idle trigger sent");
		return;
	}

	if (m_rockyController->isTargetInAttackRange())
	{
		AnimationAPI::sendTrigger(m_animation, "ToAttack");
		Debug::log("[RockyChaseState] Attack trigger sent");
		return;
	}

	m_rockyController->moveTowardsTarget();
}

void RockyChaseState::OnStateExit()
{
	Debug::log("[RockyChaseState] EXIT");

	if (!m_rockyController)
	{
		return;
	}

	m_rockyController->clearPath();
	m_rockyController->resetRepathTimer();
}

IMPLEMENT_SCRIPT(RockyChaseState)
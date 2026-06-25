#include "pch.h"
#include "SkeletonGuardState.h"

#include "SkeletonEnemyController.h"
#include "SkeletonAttackConfig.h"

SkeletonGuardState::SkeletonGuardState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void SkeletonGuardState::OnStateEnter()
{
	m_skeletonController = GameObjectAPI::findScript<SkeletonEnemyController>(getOwner());
	m_attackConfig = GameObjectAPI::findScript<SkeletonAttackConfig>(getOwner());
	m_animation = AnimationAPI::getAnimationComponent(getOwner());

	if (!m_skeletonController)
	{
		Debug::error("[SkeletonGuardState] SkeletonEnemyController not found.");
		return;
	}

	if (!m_attackConfig)
	{
		Debug::error("[SkeletonGuardState] SkeletonAttackConfig not found.");
		return;
	}

	if (!m_animation)
	{
		Debug::error("[SkeletonGuardState] AnimationComponent not found.");
		return;
	}

	m_skeletonController->clearPath();
	m_skeletonController->resetRepathTimer();
	m_skeletonController->setGuarding(true);

	Debug::log("[SkeletonGuardState] ENTER");
}

void SkeletonGuardState::OnStateUpdate()
{
	if (!m_skeletonController || !m_attackConfig || !m_animation)
	{
		return;
	}

	if (m_skeletonController->trySendDeathTrigger(m_animation))
	{
		return;
	}

	m_skeletonController->faceCurrentTarget();

	m_stateTimer += Time::getDeltaTime();

	if (m_stateTimer >= m_attackConfig->m_guardDuration)
	{
		m_skeletonController->consumeGuardCooldown();
		AnimationAPI::sendTrigger(m_animation, "ToRecover");
		return;
	}
}

void SkeletonGuardState::OnStateExit()
{
	if (m_skeletonController)
	{
		m_skeletonController->setGuarding(false);
	}

	m_stateTimer = 0.0f;

	Debug::log("[SkeletonGuardState] EXIT");
}

IMPLEMENT_SCRIPT(SkeletonGuardState)
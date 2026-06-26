#include "pch.h"
#include "SkeletonScimitarState.h"

#include "SkeletonEnemyController.h"
#include "SkeletonAttackConfig.h"
#include "EnemyAttackExecutor.h"

SkeletonScimitarState::SkeletonScimitarState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void SkeletonScimitarState::OnStateEnter()
{
	m_controller = GameObjectAPI::findScript<SkeletonEnemyController>(getOwner());
	m_attackConfig = GameObjectAPI::findScript<SkeletonAttackConfig>(getOwner());
	m_attackExecutor = GameObjectAPI::findScript<EnemyAttackExecutor>(getOwner());
	m_animation = AnimationAPI::getAnimationComponent(getOwner());

	if (!m_controller)
	{
		Debug::error("[SkeletonScimitarState] SkeletonEnemyController not found.");
		return;
	}

	if (!m_attackConfig)
	{
		Debug::error("[SkeletonScimitarState] SkeletonAttackConfig not found.");
		return;
	}

	if (!m_attackExecutor)
	{
		Debug::error("[SkeletonScimitarState] EnemyAttackExecutor not found.");
		return;
	}

	if (!m_animation)
	{
		Debug::error("[SkeletonScimitarState] AnimationComponent not found.");
		return;
	}

	m_controller->clearPath();
	m_controller->resetRepathTimer();
	m_controller->updateCurrentTarget();
	m_committedTarget = m_controller->getCurrentTarget();

	changePhase(Phase::Dash);

	Debug::log("[SkeletonScimitarState] ENTER");
}

void SkeletonScimitarState::OnStateUpdate()
{

}

void SkeletonScimitarState::OnStateExit()
{

}

void SkeletonScimitarState::changePhase(Phase phase)
{

}

void SkeletonScimitarState::updateDash()
{

}

void SkeletonScimitarState::updateAttack()
{

}

void SkeletonScimitarState::updateBackstep()
{

}

void SkeletonScimitarState::applyHit(bool shouldStun)
{

}

void SkeletonScimitarState::moveInDirection(const Vector3& direction, float speed)
{

}

void SkeletonScimitarState::goToChase()
{

}

IMPLEMENT_SCRIPT(SkeletonScimitarState)
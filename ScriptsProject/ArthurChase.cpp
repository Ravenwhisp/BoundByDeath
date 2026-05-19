#include "pch.h"
#include "ArthurChase.h"

#include "ArthurBossController.h"
#include "ArthurAttackConfig.h"

ArthurChase::ArthurChase(GameObject* owner)
	: StateMachineScript(owner)
{
}

void ArthurChase::OnStateEnter()
{
	m_arthurController = GameObjectAPI::findScript<ArthurBossController>(getOwner());
	m_arthurAttackConfig = GameObjectAPI::findScript<ArthurAttackConfig>(getOwner());

	if (!m_arthurController)
	{
		Debug::error("[ArthurChase] ArthurBossController not found.");
		return;
	}

	if (!m_arthurAttackConfig)
	{
		Debug::error("[ArthurChase] ArthurAttackConfig not found.");
		return;
	}

	m_arthurController->clearPath();
	m_arthurController->resetRepathTimer();
	m_arthurController->updateCurrentTarget();

	if (m_arthurController->hasValidTarget())
	{
		m_arthurController->buildPathToTarget();
	}

	Debug::log("[ArthurChase] ENTER");
}

void ArthurChase::OnStateUpdate()
{

	if (!m_arthurController)
	{
		Debug::error("[ArthurChase] ArthurBossController not found.");
		return;
	}

	if (!m_arthurAttackConfig)
	{
		Debug::error("[ArthurChase] ArthurAttackConfig not found.");
		return;
	}

	AnimationComponent* animation = AnimationAPI::getAnimationComponent(getOwner());
	if (!animation)
	{
		Debug::error("[ArthurChase] Animation Component not found.");
		return;
	}

	m_arthurController->updateCurrentTarget();

	if (!m_arthurController->hasValidTarget())
	{
		m_arthurController->clearPath();
		return;
	}

	// Phase check
	if (m_arthurController->getPhase() == ArthurBossPhase::Phase1)
	{

		// Attack checks + State transitions

		// Side Sweep
		if (m_arthurController->trySelectSideSweepSide())
		{
			m_arthurController->clearPath();
			//m_arthurController->faceCurrentTarget();

			AnimationAPI::sendTrigger(animation, "ToSideSweep");
			return;
		}

		float targetDistance = m_arthurController->getDistanceToCurrentTarget();

		// Heavy Swipe
		if (targetDistance <= m_arthurAttackConfig->m_heavySwipeRange)
		{
			m_arthurController->clearPath();
			m_arthurController->faceCurrentTarget();

			AnimationAPI::sendTrigger(animation, "ToHeavySwipe");
			return;
		}

	}
	//else if (m_arthurController->getPhase() == ArthurBossPhase::Phase2)
	//{

	//}

	m_arthurController->addToRepathTimer(Time::getDeltaTime());

	if (m_arthurController->shouldRepath())
	{
		m_arthurController->buildPathToTarget();
		m_arthurController->resetRepathTimer();
	}

	m_arthurController->followPath();
}

void ArthurChase::OnStateExit()
{
	if (!m_arthurController)
	{
		return;
	}

	m_arthurController->clearPath();
	m_arthurController->resetRepathTimer();

	Debug::log("[ArthurChase] EXIT");
}

IMPLEMENT_SCRIPT(ArthurChase)
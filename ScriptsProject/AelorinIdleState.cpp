#include "pch.h"
#include "AelorinIdleState.h"

#include "AelorinBossController.h"

AelorinIdleState::AelorinIdleState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void AelorinIdleState::OnStateEnter()
{
	Transform* parentTransform = TransformAPI::getParent(getOwner()->GetTransform());
	GameObject* parentGameObject = ComponentAPI::getOwner(parentTransform);

	m_controller = GameObjectAPI::findScript<AelorinBossController>(parentGameObject);
	m_animation = AnimationAPI::getAnimationComponent(getOwner());

	m_decisionTimer = 0.0f;
	m_decisionMade = false;

	if (!m_controller)
	{
		Debug::error("[AelorinIdleState] AelorinBossController not found.");
	}

	if (!m_animation)
	{
		Debug::error("[AelorinIdleState] AnimationComponent not found.");
	}

	Debug::log("[AelorinIdleState] ENTER");
}

void AelorinIdleState::OnStateUpdate()
{
	if (!m_controller || !m_animation)
	{
		return;
	}

	if (m_controller->trySendPhaseTransitionTrigger(m_animation))
	{
		return;
	}

	if (m_controller->trySendThresholdStaggerTrigger(m_animation))
	{
		return;
	}

	if (m_decisionMade)
	{
		return;
	}

	m_decisionTimer += Time::getDeltaTime();

	if (m_decisionTimer < m_controller->getDecisionTime())
	{
		return;
	}

	m_decisionMade = true;

	Debug::log("[AelorinIdleState] Decision time elapsed.");
}

void AelorinIdleState::OnStateExit()
{
	Debug::log("[AelorinIdleState] EXIT");
}

IMPLEMENT_SCRIPT(AelorinIdleState)
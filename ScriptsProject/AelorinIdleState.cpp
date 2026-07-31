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
}

void AelorinIdleState::OnStateExit()
{
	Debug::log("[AelorinIdleState] EXIT");
}

IMPLEMENT_SCRIPT(AelorinIdleState)
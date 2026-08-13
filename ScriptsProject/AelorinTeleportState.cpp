#include "pch.h"
#include "AelorinTeleportState.h"

#include "AelorinBossController.h"

AelorinTeleportState::AelorinTeleportState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void AelorinTeleportState::OnStateEnter()
{
	Transform* parentTransform = TransformAPI::getParent(getOwner()->GetTransform());
	if (!parentTransform)
	{
		Debug::error("[AelorinTeleportState] Aelorin transform not found.");
		return;
	}

	GameObject* parentGameObject = ComponentAPI::getOwner(parentTransform);

	m_controller = GameObjectAPI::findScript<AelorinBossController>(parentGameObject);
	m_animation = AnimationAPI::getAnimationComponent(getOwner());

	m_controller = GameObjectAPI::findScript<AelorinBossController>(parentGameObject);
	m_animation = AnimationAPI::getAnimationComponent(getOwner());

	if (!m_controller)
	{
		Debug::error("[AelorinTeleportState] AelorinBossController not found.");
	}

	if (!m_animation)
	{
		Debug::error("[AelorinTeleportState] AnimationComponent not found.");
	}

	Debug::log("[AelorinTeleportState] ENTER");
}

void AelorinTeleportState::OnStateUpdate()
{
}

void AelorinTeleportState::OnStateExit()
{
	Debug::log("[AelorinTeleportState] EXIT");
}

IMPLEMENT_SCRIPT(AelorinTeleportState)
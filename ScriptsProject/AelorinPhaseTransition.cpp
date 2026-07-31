#include "pch.h"
#include "AelorinPhaseTransition.h"

#include "AelorinBossController.h"

AelorinPhaseTransition::AelorinPhaseTransition(GameObject* owner)
	: StateMachineScript(owner)
{
}

void AelorinPhaseTransition::OnStateEnter()
{
	Transform* parentTransform = TransformAPI::getParent(getOwner()->GetTransform());
	GameObject* parentGameObject = ComponentAPI::getOwner(parentTransform);

	m_controller = GameObjectAPI::findScript<AelorinBossController>(parentGameObject);
	m_animation = AnimationAPI::getAnimationComponent(getOwner());

	m_phase2Started = false;

	if (!m_controller)
	{
		Debug::error("[AelorinPhaseTransition] AelorinBossController not found.");
	}

	if (!m_animation)
	{
		Debug::error("[AelorinPhaseTransition] AnimationComponent not found.");
	}

	Debug::log("[AelorinPhaseTransition] ENTER");
}

void AelorinPhaseTransition::OnStateUpdate()
{
	if (!m_controller || !m_animation)
	{
		return;
	}

	if (m_phase2Started)
	{
		return;
	}

	if (!AnimationAPI::isPlaying(m_animation))
	{
		Debug::log("[AelorinPhaseTransition] stopped playing animation");
		m_phase2Started = true;
		m_controller->beginPhase2();
	}
}

void AelorinPhaseTransition::OnStateExit()
{
	Debug::log("[AelorinPhaseTransition] EXIT");
}

IMPLEMENT_SCRIPT(AelorinPhaseTransition)
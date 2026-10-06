#include "pch.h"
#include "AelorinPhaseCinematicState.h"

#include "AelorinBossController.h"
#include "AelorinCinematics.h"

AelorinPhaseCinematicState::AelorinPhaseCinematicState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void AelorinPhaseCinematicState::OnStateEnter()
{
	Transform* parentTransform = TransformAPI::getParent(getOwner()->GetTransform());
	if (!parentTransform)
	{
		Debug::error("[AelorinPhaseCinematicState] Aelorin transform not found.");
		return;
	}

	GameObject* parentGameObject = ComponentAPI::getOwner(parentTransform);
	m_controller = GameObjectAPI::findScript<AelorinBossController>(parentGameObject);
	m_cinematics = GameObjectAPI::findScript<AelorinCinematics>(parentGameObject);
	m_animation = AnimationAPI::getAnimationComponent(getOwner());

	m_transitionSent = false;

	if (!m_controller)
	{
		Debug::error("[AelorinPhaseCinematicState] AelorinBossController not found.");
		return;
	}

	if (!m_animation)
	{
		Debug::error("[AelorinPhaseCinematicState] AnimationComponent not found.");
		return;
	}

	if (!m_cinematics)
	{
		Debug::warn("[AelorinPhaseCinematicState] AelorinCinematics not found.");
		return;
	}

	const bool started = m_cinematics->startPhaseTransition();

	if (!started)
	{
		Debug::warn("[AelorinPhaseCinematicState] Phase cinematic failed to start.");
	}

	Debug::log("[AelorinPhaseCinematicState] ENTER");
}

void AelorinPhaseCinematicState::OnStateUpdate()
{
	if (!m_animation ||	m_transitionSent)
	{
		return;
	}

	if (!m_cinematics)
	{
		return;
	}

	if (!m_cinematics->isPhaseTeleportFinished())
	{
		return;
	}

	m_transitionSent = true;

	const bool sent = AnimationAPI::sendTrigger(m_animation, "ToTransformation");

	if (!sent)
	{
		Debug::warn("[AelorinPhaseCinematicState] Failed to send ToTransformation.");

		m_transitionSent = false;
		return;
	}

	Debug::log("[AelorinPhaseCinematicState] Teleport cinematic finished -> Transformation.");
}

void AelorinPhaseCinematicState::OnStateExit()
{
	Debug::log("[AelorinPhaseCinematicState] EXIT");
}

IMPLEMENT_SCRIPT(AelorinPhaseCinematicState)
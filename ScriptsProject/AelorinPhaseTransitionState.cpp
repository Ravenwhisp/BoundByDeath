#include "pch.h"
#include "AelorinPhaseTransitionState.h"

#include "AelorinBossController.h"
#include "AelorinVFX.h"
#include "AelorinCinematics.h"

AelorinPhaseTransitionState::AelorinPhaseTransitionState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void AelorinPhaseTransitionState::OnStateEnter()
{
	Transform* parentTransform = TransformAPI::getParent(getOwner()->GetTransform());
	if (!parentTransform)
	{
		Debug::error("[AelorinPhaseTransitionState] Aelorin transform not found.");
		return;
	}

	GameObject* parentGameObject = ComponentAPI::getOwner(parentTransform);

	m_controller = GameObjectAPI::findScript<AelorinBossController>(parentGameObject);
	m_animation = AnimationAPI::getAnimationComponent(getOwner());
	m_vfx = GameObjectAPI::findScript<AelorinVFX>(parentGameObject);

	m_phase2Started = false;
	m_cinematicDriven = false;

	if (!m_controller)
	{
		Debug::error("[AelorinPhaseTransitionState] AelorinBossController not found.");
		return;
	}

	if (!m_animation)
	{
		Debug::error("[AelorinPhaseTransitionState] AnimationComponent not found.");
		return;
	}

	if (m_vfx)
	{
		m_vfx->playPhase2Transition();
	}

	// Con cinemática montada, ella lleva el reloj: mueve al boss al centro, lanza el hechizo y
	// dispara el cambio de modelo tapado por el pilar de almas. Sin cinemática, se espera a que
	// acabe la animación como siempre.
	AelorinCinematics* cinematics = GameObjectAPI::findScript<AelorinCinematics>(parentGameObject);
	if (cinematics)
	{
		m_cinematicDriven = cinematics->startPhaseTransition();
	}

	Debug::log("[AelorinPhaseTransitionState] ENTER");
}

void AelorinPhaseTransitionState::OnStateUpdate()
{
	if (!m_controller || !m_animation)
	{
		return;
	}

	if (m_phase2Started || m_cinematicDriven)
	{
		return;
	}

	if (!AnimationAPI::isPlaying(m_animation))
	{
		Debug::log("[AelorinPhaseTransitionState] stopped playing animation");
		m_phase2Started = true;
		m_controller->beginPhase2();
	}
}

void AelorinPhaseTransitionState::OnStateExit()
{
	Debug::log("[AelorinPhaseTransitionState] EXIT");
}

IMPLEMENT_SCRIPT(AelorinPhaseTransitionState)
#include "pch.h"
#include "AelorinExhaustionState.h"

#include "AelorinBossController.h"
#include "AelorinAttackConfig.h"

AelorinExhaustionState::AelorinExhaustionState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void AelorinExhaustionState::OnStateEnter()
{
	Transform* parentTransform = TransformAPI::getParent(getOwner()->GetTransform());
	if (!parentTransform)
	{
		Debug::error("[AelorinExhaustionState] Aelorin transform not found.");
		return;
	}

	GameObject* parentGameObject = ComponentAPI::getOwner(parentTransform);

	// get scripts
	m_controller = GameObjectAPI::findScript<AelorinBossController>(parentGameObject);
	m_animation = AnimationAPI::getAnimationComponent(getOwner());

	// reset members
	m_stateTimer = 0.0f;
	m_completed = false;

	if (!m_controller)
	{
		Debug::error("[AelorinExhaustionState] AelorinBossController not found.");
		return;
	}

	if (!m_animation)
	{
		Debug::error("[AelorinExhaustionState] AnimationComponent not found.");
		return;
	}

	if (m_controller && m_controller->isPhase2())
	{
		changePhase(ExhaustionPhase::Loop);
	}

	Debug::log("[AelorinExhaustionState] ENTER");
}

void AelorinExhaustionState::OnStateUpdate()
{
	if (!m_controller || !m_animation || m_completed)
	{
		return;
	}

	if (m_controller->trySendPriorityInterrupt(m_animation))
	{
		return;
	}

	const AelorinAttackConfig* config = m_controller->getAelorinAttackConfig();
	if (!config)
	{
		return;
	}

	m_stateTimer += Time::getDeltaTime();

	if (m_phase == ExhaustionPhase::Loop)
	{
		if (m_stateTimer < config->m_furyExhaustionDuration)
		{
			return;
		}

		changePhase(ExhaustionPhase::Transition);
		return;
	}

	if (m_phase == ExhaustionPhase::Transition)
	{
		constexpr float transitionDuration = 1.33f;

		if (m_stateTimer < transitionDuration)
		{
			return;
		}

		finishExhaustion();
	}
}

void AelorinExhaustionState::OnStateExit()
{
	m_stateTimer = 0.0f;
	m_completed = false;

	Debug::log("[AelorinExhaustionState] EXIT");
}

void AelorinExhaustionState::changePhase(ExhaustionPhase phase)
{
	m_phase = phase;
	m_stateTimer = 0.0f;

	if (!m_animation)
	{
		return;
	}

	if (phase == ExhaustionPhase::Loop)
	{
		AnimationAPI::playOverrideClip(m_animation, "exhaustloop_phase2", 0.0f, true);
		AnimationAPI::setPlaybackTime(m_animation, 0.0f);
		return;
	}

	if (phase == ExhaustionPhase::Transition)
	{
		AnimationAPI::playOverrideClip(m_animation, "exhausttrans_phase2", 0.0f, false);
		AnimationAPI::setPlaybackTime(m_animation, 0.0f);
		return;
	}
}

void AelorinExhaustionState::finishExhaustion()
{
	if (m_completed)
	{
		return;
	}

	m_completed = true;

	// Fury officialy ends after vulnerability window here
	m_controller->finishFury();

	const bool sent = AnimationAPI::sendTrigger(m_animation, "ToIdle");
	if (!sent)
	{
		Debug::warn("[AelorinExhaustionState] Failed to send ToIdle trigger");
	}
}

IMPLEMENT_SCRIPT(AelorinExhaustionState)
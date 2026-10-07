#include "pch.h"
#include "AelorinPhaseTransitionState.h"

#include "AelorinBossController.h"
#include "AelorinVFX.h"
#include "AelorinDamageable.h"

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
	m_damageable = GameObjectAPI::findScript<AelorinDamageable>(parentGameObject);

	m_dissolveStarted = false;
	m_phase2RevealStarted = false;

	m_phase2RevealTimer = 0.0f;

	m_phase2Started = false;

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

	Debug::log("[AelorinPhaseTransitionState] ENTER");
}

void AelorinPhaseTransitionState::OnStateUpdate()
{
	if (!m_controller || !m_animation || !m_damageable || m_phase2Started)
	{
		return;
	}

	const float playbackTime = AnimationAPI::getPlaybackTime(m_animation);
	const float playbackDuration = AnimationAPI::getPlaybackDuration(m_animation);

	if (playbackDuration <= 0.001f)
	{
		return;
	}

	const float animationProgress = playbackTime / playbackDuration;

	// Start Phase 1 dissolve
	if (!m_dissolveStarted && animationProgress >= 0.5f)
	{
		m_dissolveStarted = true;
		m_phase2RevealTimer = 0.0f;

		m_damageable->startPhase1Dissolve();
	}

	// Wait so Phase 2 appears underneath
	if (m_dissolveStarted && !m_phase2RevealStarted)
	{
		m_phase2RevealTimer += Time::getDeltaTime();

		constexpr float phase2RevealDelay = 0.2f;
		if (m_phase2RevealTimer >= phase2RevealDelay)
		{
			m_phase2RevealStarted = true;
			m_controller->showPhase2TransitionModel();
		}
	}

	// Phase 1 now gone
	if (m_phase2RevealStarted && m_damageable->isDissolveFinished())
	{
		m_phase2Started = true;
		m_controller->beginPhase2();
	}
}

void AelorinPhaseTransitionState::OnStateExit()
{
	m_dissolveStarted = false;
	m_phase2RevealStarted = false;
	m_phase2RevealTimer = 0.0f;
	m_phase2Started = false;

	Debug::log("[AelorinPhaseTransitionState] EXIT");
}

IMPLEMENT_SCRIPT(AelorinPhaseTransitionState)
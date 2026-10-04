#include "pch.h"
#include "AelorinThresholdStaggerState.h"

#include "AelorinBossController.h"

AelorinThresholdStaggerState::AelorinThresholdStaggerState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void AelorinThresholdStaggerState::OnStateEnter()
{
	Transform* parentTransform = TransformAPI::getParent(getOwner()->GetTransform());
	if (!parentTransform)
	{
		Debug::error("[AelorinThresholdStaggerState] Aelorin transform not found.");
		return;
	}

	GameObject* parentGameObject = ComponentAPI::getOwner(parentTransform);

	m_controller = GameObjectAPI::findScript<AelorinBossController>(parentGameObject);
	m_animation = AnimationAPI::getAnimationComponent(getOwner());
	m_staggerCompleted = false;
	m_staggerTimer = 0.0f;

	if (!m_controller)
	{
		Debug::error("[AelorinThresholdStaggerState] AelorinBossController not found.");
		return;
	}

	if (!m_animation)
	{
		Debug::error("[AelorinThresholdStaggerState] AnimationComponent not found.");
		return;
	}

	if (m_controller && m_controller->isPhase2())
	{
		changePhase(StaggerPhase::Stagger);
	}

	Debug::log("[AelorinThresholdStaggerState] ENTER");
}

void AelorinThresholdStaggerState::OnStateUpdate()
{
	if (!m_controller || !m_animation || m_staggerCompleted)
	{
		return;
	}

	m_staggerTimer += Time::getDeltaTime();

	// PHASE 1
	if (!m_controller->isPhase2())
	{
		if (m_staggerTimer < m_controller->getThresholdStaggerDuration())
		{
			return;
		}

		finishStagger();
		return;
	}

	// PHASE 2
	if (m_phase == StaggerPhase::Stagger)
	{
		if (m_staggerTimer < m_controller->getThresholdStaggerDuration())
		{
			return;
		}

		changePhase(StaggerPhase::StaggerOut);
		return;
	}

	if (m_phase == StaggerPhase::StaggerOut)
	{
		constexpr float staggerOutDuration = 0.65f;
		if (m_staggerTimer < staggerOutDuration)
		{
			return;
		}

		finishStagger();
	}
}

void AelorinThresholdStaggerState::OnStateExit()
{
	if (m_animation && m_controller && m_controller->isPhase2())
	{
		AnimationAPI::clearOverrideClip(m_animation, 0.0f);
	}

	m_staggerTimer = 0.0f;
	m_staggerCompleted = false;
	Debug::log("[AelorinThresholdStaggerState] EXIT");
}

void AelorinThresholdStaggerState::changePhase(StaggerPhase phase)
{
	m_phase = phase;
	m_staggerTimer = 0.0f;

	if (!m_animation)
	{
		return;
	}

	if (phase == StaggerPhase::Stagger)
	{
		AnimationAPI::playOverrideClip(m_animation, "stagger_phase2", 0.0f,	false);
		AnimationAPI::setPlaybackTime(m_animation, 0.0f);
		return;
	}

	if (phase == StaggerPhase::StaggerOut)
	{
		AnimationAPI::playOverrideClip(m_animation,	"staggerout_phase2", 0.0f, false);
		AnimationAPI::setPlaybackTime(m_animation, 0.0f);
		return;
	}
}

void AelorinThresholdStaggerState::finishStagger()
{
	if (m_staggerCompleted)
	{
		return;
	}

	m_staggerCompleted = true;
	m_controller->completeThresholdStagger();

	if (m_controller->isPhase2())
	{
		AnimationAPI::clearOverrideClip(m_animation, 0.0f);
	}

	const bool sent = AnimationAPI::sendTrigger(m_animation, "ToIdle");

	if (!sent)
	{
		Debug::warn("[AelorinThresholdStaggerState] Failed to send ToIdle trigger.");
	}
}

IMPLEMENT_SCRIPT(AelorinThresholdStaggerState)
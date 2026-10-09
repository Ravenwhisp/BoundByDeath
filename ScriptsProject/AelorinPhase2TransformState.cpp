#include "pch.h"
#include "AelorinPhase2TransformState.h"

#include "AelorinBossController.h"

AelorinPhase2TransformState::AelorinPhase2TransformState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void AelorinPhase2TransformState::OnStateEnter()
{
	Transform* parentTransform = TransformAPI::getParent(getOwner()->GetTransform());
	if (!parentTransform)
	{
		Debug::error("[AelorinPhase2TransformState] Aelorin transform not found.");
		return;
	}

	GameObject* parentGameObject = ComponentAPI::getOwner(parentTransform);

	m_controller = GameObjectAPI::findScript<AelorinBossController>(parentGameObject);
	m_animation = AnimationAPI::getAnimationComponent(getOwner());
	m_modelTransform = TransformAPI::findChildByName(parentTransform, "Phase2");

	// reset members
	m_stateTimer = 0.0f;
	m_completed = false;
	m_dissolve = nullptr;

	if (!m_controller)
	{
		Debug::error("[AelorinPhase2TransformState] AelorinBossController not found.");
		return;
	}

	if (!m_animation)
	{
		Debug::error("[AelorinPhase2TransformState] AnimationComponent not found.");
		return;
	}

	if (!m_modelTransform)
	{
		Debug::error("[AelorinPhase2TransformState] Phase2 model not found.");
		return;
	}

	m_targetScale = TransformAPI::getScale(m_modelTransform);
	constexpr float startScaleFactor = 0.4f;

	m_startScale = Vector3(
		m_targetScale.x * startScaleFactor,
		m_targetScale.y * startScaleFactor,
		m_targetScale.z * startScaleFactor
	);

	TransformAPI::setScale(m_modelTransform, m_startScale);

	Transform* golemTransform = TransformAPI::findChildByName(m_modelTransform, "Golem");
	if (!golemTransform)
	{
		Debug::warn("[AelorinPhase2TransformState] Golem not found under Phase2.");
	}
	else
	{
		GameObject* golemObject = ComponentAPI::getOwner(golemTransform);
		m_dissolve = ShadersAPI::getDissolveComponent(golemObject);

		if (!m_dissolve)
		{
			Debug::warn("[AelorinPhase2TransformState] DissolveComponent not found on Golem.");
		}
		else
		{
			// Completely dissolved/invisible at the start.
			ShadersAPI::setDissolveAmount(m_dissolve, 1.0f);
		}
	}


	Debug::log("[AelorinPhase2TransformState] ENTER");
}

void AelorinPhase2TransformState::OnStateUpdate()
{
	if (!m_controller || !m_animation || !m_modelTransform || m_completed)
	{
		return;
	}

	constexpr float transformDuration = 2.5f;
	constexpr float delay = 1.0f;

	m_stateTimer += Time::getDeltaTime();

	if (m_stateTimer <= delay)
	{
		return;
	}

	float t = (m_stateTimer - delay) / transformDuration;

	if (t > 1.0f)
	{
		t = 1.0f;
	}

	// Smoothstep
	const float smoothT = t * t * (3.0f - 2.0f * t);

	const Vector3 currentScale(
		m_startScale.x + (m_targetScale.x - m_startScale.x) * smoothT,
		m_startScale.y + (m_targetScale.y - m_startScale.y) * smoothT,
		m_startScale.z + (m_targetScale.z - m_startScale.z) * smoothT
	);

	TransformAPI::setScale(m_modelTransform, currentScale);

	if (m_dissolve)
	{
		const float dissolveAmount = 1.0f - smoothT;

		ShadersAPI::setDissolveAmount(m_dissolve, dissolveAmount);
	}

	if (t < 1.0f)
	{
		return;
	}

	// Set final scale
	TransformAPI::setScale(m_modelTransform, m_targetScale);
	if (m_dissolve)
	{
		ShadersAPI::setDissolveAmount(m_dissolve, 0.0f);
	}

	m_completed = true;

	const bool sent = AnimationAPI::sendTrigger(m_animation, "ToIdle");
	if (!sent)
	{
		Debug::warn("[AelorinPhase2TransformState] Failed to send ToIdle trigger.");
	}
}

void AelorinPhase2TransformState::OnStateExit()
{
	if (m_modelTransform)
	{
		TransformAPI::setScale(m_modelTransform, m_targetScale);
	}

	if (m_dissolve)
	{
		ShadersAPI::setDissolveAmount(m_dissolve, 0.0f);
	}

	m_modelTransform = nullptr;
	m_dissolve = nullptr;

	m_stateTimer = 0.0f;
	m_completed = false;

	Debug::log("[AelorinPhase2TransformState] EXIT");
}

IMPLEMENT_SCRIPT(AelorinPhase2TransformState)
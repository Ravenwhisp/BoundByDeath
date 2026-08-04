#include "pch.h"
#include "AelorinThresholdStagger.h"

#include "AelorinBossController.h"

AelorinThresholdStagger::AelorinThresholdStagger(GameObject* owner)
	: StateMachineScript(owner)
{
}

void AelorinThresholdStagger::OnStateEnter()
{
	Transform* parentTransform = TransformAPI::getParent(getOwner()->GetTransform());
	GameObject* parentGameObject = ComponentAPI::getOwner(parentTransform);

	m_controller = GameObjectAPI::findScript<AelorinBossController>(parentGameObject);
	m_animation = AnimationAPI::getAnimationComponent(getOwner());
	m_staggerCompleted = false;
	m_staggerTimer = 0.0f;

	if (!m_controller)
	{
		Debug::error("[AelorinThresholdStagger] AelorinBossController not found.");
	}

	if (!m_animation)
	{
		Debug::error("[AelorinThresholdStagger] AnimationComponent not found.");
	}

	Debug::log("[AelorinThresholdStagger] ENTER");
}

void AelorinThresholdStagger::OnStateUpdate()
{
	if (!m_controller || !m_animation || m_staggerCompleted)
	{
		return;
	}

	m_staggerTimer += Time::getDeltaTime();

	if (m_staggerTimer < m_controller->getThresholdStaggerDuration())
	{
		return;
	}

	m_staggerCompleted = true;

	m_controller->completeThresholdStagger();

	const bool sent = AnimationAPI::sendTrigger(m_animation, "ToIdle");

	if (!sent)
	{
		Debug::warn("[AelorinThresholdStagger] Failed to send ToIdle trigger.");
	}
}

void AelorinThresholdStagger::OnStateExit()
{
	Debug::log("[AelorinThresholdStagger] EXIT");
}

IMPLEMENT_SCRIPT(AelorinThresholdStagger)
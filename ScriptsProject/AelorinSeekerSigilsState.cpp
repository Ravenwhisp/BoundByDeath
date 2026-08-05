#include "pch.h"
#include "AelorinSeekerSigilsState.h"

AelorinSeekerSigilsState::AelorinSeekerSigilsState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void AelorinSeekerSigilsState::OnStateEnter()
{
	Transform* parentTransform = TransformAPI::getParent(getOwner()->GetTransform());
	GameObject* parentGameObject = ComponentAPI::getOwner(parentTransform);

	m_controller = GameObjectAPI::findScript<AelorinBossController>(parentGameObject);
	m_animation = AnimationAPI::getAnimationComponent(getOwner());

	m_activeAbility = AelorinAbility::None;
	m_testTimer = 0.0f;
	m_completed = false;

	if (!m_controller)
	{
		Debug::error("[AelorinSeekerSigilsState] AelorinBossController not found.");
	}

	if (!m_animation)
	{
		Debug::error("[AelorinSeekerSigilsState] AnimationComponent not found.");
	}

	m_activeAbility = m_controller->consumeRequestedAbility();
	if (m_activeAbility != AelorinAbility::SeekerSigils)
	{
		Debug::warn("[AelorinSeekerSigilsState] Unexpected requested ability!");
	}

	Debug::log("[AelorinSeekerSigilsState] ENTER");
}

void AelorinSeekerSigilsState::OnStateUpdate()
{
	if (!m_controller || !m_animation || m_completed)
	{
		return;
	}

	m_testTimer += Time::getDeltaTime();

	if (m_testTimer < 1.0f)
	{
		return;
	}

	m_completed = true;

	const bool sent = AnimationAPI::sendTrigger(m_animation, "ToIdle");
	if (!sent)
	{
		Debug::warn("[AelorinSeekerSigilsState] Failed to send ToIdle trigger.");
	}
}

void AelorinSeekerSigilsState::OnStateExit()
{
	Debug::log("[AelorinSeekerSigilsState] EXIT");
}

IMPLEMENT_SCRIPT(AelorinSeekerSigilsState)
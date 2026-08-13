#include "pch.h"
#include "AelorinRisenSpiresState.h"

#include "AelorinAttackConfig.h"
#include "AelorinAttackExecutor.h"

AelorinRisenSpiresState::AelorinRisenSpiresState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void AelorinRisenSpiresState::OnStateEnter()
{
	Transform* parentTransform = TransformAPI::getParent(getOwner()->GetTransform());
	if (!parentTransform)
	{
		Debug::error("[AelorinRisenSpiresState] Aelorin transform not found.");
		return;
	}

	GameObject* parentGameObject = ComponentAPI::getOwner(parentTransform);

	// get scripts
	m_controller = GameObjectAPI::findScript<AelorinBossController>(parentGameObject);
	m_animation = AnimationAPI::getAnimationComponent(getOwner());

	// reset members
	m_activeAbility = AelorinAbility::None;
	m_stateTimer = 0.0f;
	m_firstPassExecuted = false;
	m_secondPassExecuted = false;
	m_completed = false;

	if (!m_controller)
	{
		Debug::error("[AelorinRisenSpiresState] AelorinBossController not found.");
		return;
	}

	if (!m_animation)
	{
		Debug::error("[AelorinRisenSpiresState] AnimationComponent not found.");
		return;
	}

	m_attackExecutor = m_controller->getAttackExecutor();

	if (!m_attackExecutor)
	{
		Debug::error("[AelorinRisenSpiresState] AelorinAttackExecutor not found.");
		return;
	}

	// consume ability
	m_activeAbility = m_controller->consumeRequestedAbility();
	if (m_activeAbility != AelorinAbility::RisenSpires)
	{
		Debug::warn("[AelorinRisenSpiresState] Unexpected requested ability!");
		return;
	}

	Debug::log("[AelorinRisenSpiresState] ENTER");
}

void AelorinRisenSpiresState::OnStateUpdate()
{
	if (!m_controller || !m_attackExecutor || !m_animation || m_completed)
	{
		return;
	}

	const AelorinAttackConfig* config = m_controller->getAelorinAttackConfig();
	if (!config)
	{
		return;
	}

	m_stateTimer += Time::getDeltaTime();

	// First pass after the 3 second windup
	if (!m_firstPassExecuted && m_stateTimer >= config->m_risenSpiresWindupDuration)
	{
		executePattern(m_controller->getRisenSpiresPatternARoot(), "Risen Spires Pass 1");
		m_firstPassExecuted = true;
	}

	// Phase 2 gets opposite pattern 2 seconds later
	if (m_controller->isPhase2() && m_firstPassExecuted && !m_secondPassExecuted && m_stateTimer >= config->m_risenSpiresWindupDuration + config->m_risenSpiresPhase2SecondPassDelay)
	{
		executePattern(m_controller->getRisenSpiresPatternBRoot(), "Risen Spires Pass 2");
		m_secondPassExecuted = true;
	}

	const float lastPassTime = m_controller->isPhase2() ? config->m_risenSpiresWindupDuration + config->m_risenSpiresPhase2SecondPassDelay : config->m_risenSpiresWindupDuration;

	if (m_stateTimer < lastPassTime + config->m_risenSpiresRecoveryDuration)
	{
		return;
	}

	finishAbility();
}

void AelorinRisenSpiresState::OnStateExit()
{
	m_stateTimer = 0.0f;
	m_firstPassExecuted = false;
	m_secondPassExecuted = false;
	m_completed = false;

	Debug::log("[AelorinRisenSpiresState] EXIT");
}

void AelorinRisenSpiresState::executePattern(Transform* patternRoot, const char* sourceName)
{
	if (!patternRoot || !m_attackExecutor)
	{
		return;
	}

	const AelorinAttackConfig* config = m_controller->getAelorinAttackConfig();
	if (!config)
	{
		return;
	}

	// pattern has child game objects and uses their transform to position the attack
	const int childCount = TransformAPI::getChildCount(patternRoot);

	for (int i = 0; i < childCount; ++i)
	{
		Transform* spirePoint = TransformAPI::getChild(patternRoot, i);
		if (!spirePoint)
		{
			continue;
		}

		const Vector3 position = TransformAPI::getGlobalPosition(spirePoint);

		m_attackExecutor->applyDamageInRadius(position, config->m_risenSpiresRadius, config->m_risenSpiresDamage, sourceName);
	}

	Debug::log("[AelorinRisenSpiresState] Executed pattern with %d spires", childCount);
}

void AelorinRisenSpiresState::finishAbility()
{
	if (m_completed)
	{
		return;
	}

	m_completed = true;

	const bool sent = AnimationAPI::sendTrigger(m_animation, "ToIdle");
	if (!sent)
	{
		Debug::warn("[AelorinRisenSpiresState] Failed to send ToIdle trigger");
	}
}

IMPLEMENT_SCRIPT(AelorinRisenSpiresState)
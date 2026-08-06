#include "pch.h"
#include "AelorinNovaState.h"
#include "AelorinAttackExecutor.h"

#include "AelorinAttackConfig.h"

AelorinNovaState::AelorinNovaState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void AelorinNovaState::OnStateEnter()
{
	Transform* parentTransform = TransformAPI::getParent(getOwner()->GetTransform());
	GameObject* parentGameObject = ComponentAPI::getOwner(parentTransform);

	// get scripts
	m_controller = GameObjectAPI::findScript<AelorinBossController>(parentGameObject);
	m_animation = AnimationAPI::getAnimationComponent(getOwner());

	// reset members
	m_activeAbility = AelorinAbility::None;
	m_stateTimer = 0.0f;
	m_firstWaveApplied = false;
	m_secondWaveApplied = false;
	m_completed = false;

	if (!m_controller)
	{
		Debug::error("[AelorinNovaState] AelorinBossController not found.");
	}

	if (!m_animation)
	{
		Debug::error("[AelorinNovaState] AnimationComponent not found.");
	}

	// consume ability
	m_activeAbility = m_controller->consumeRequestedAbility();
	if (m_activeAbility != AelorinAbility::Nova)
	{
		Debug::warn("[AelorinNovaState] Unexpected requested ability!");
	}

	Debug::log("[AelorinNovaState] ENTER");
}

void AelorinNovaState::OnStateUpdate()
{
	if (!m_controller || !m_animation ||  m_completed)
	{
		return;
	}

	const AelorinAttackConfig* config = m_controller->getAelorinAttackConfig();
	if (!config)
	{
		return;
	}

	m_stateTimer += Time::getDeltaTime();

	if (!m_firstWaveApplied && m_stateTimer >= config->m_novaChargeTime)
	{
		executeFirstNovaWave();
		m_firstWaveApplied = true;
	}

	if (m_controller->isPhase2() &&
		m_firstWaveApplied &&
		!m_secondWaveApplied &&
		m_stateTimer >= config->m_novaChargeTime + config->m_novaPhase2SecondWaveDelay)
	{
		executeSecondNovaWave();
		m_secondWaveApplied = true;
	}

	const float lastWaveTime = m_controller->isPhase2() ? config->m_novaChargeTime + config->m_novaPhase2SecondWaveDelay : config->m_novaChargeTime;

	if (m_stateTimer < lastWaveTime + config->m_novaRecoveryDuration)
	{
		return;
	}

	finishAbility();
}

void AelorinNovaState::OnStateExit()
{
	m_stateTimer = 0.0f;
	m_firstWaveApplied = false;
	m_secondWaveApplied = false;
	m_completed = false;

	Debug::log("[AelorinNovaState] EXIT");
}

void AelorinNovaState::executeFirstNovaWave()
{
	if (!m_controller)
	{
		return;
	}

	const AelorinAttackConfig* config = m_controller->getAelorinAttackConfig();
	if (!config)
	{
		return;
	}

	executeNovaWave(config->m_novaRadius, config->m_novaDamage);

	Debug::log("[AelorinNovaState] First Nova wave.");
}

void AelorinNovaState::executeSecondNovaWave()
{
	if (!m_controller)
	{
		return;
	}

	const AelorinAttackConfig* config = m_controller->getAelorinAttackConfig();
	if (!config)
	{
		return;
	}

	executeNovaWave(config->m_novaPhase2SecondRadius, config->m_novaPhase2SecondDamage);

	Debug::log("[AelorinNovaState] Second Nova wave.");
}

void AelorinNovaState::executeNovaWave(float radius, float damage)
{
	AelorinAttackExecutor* executor = m_controller->getAttackExecutor();

	if (!executor)
	{
		return;
	}

	Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
	if (!ownerTransform)
	{
		return;
	}

	const Vector3 center = TransformAPI::getGlobalPosition(ownerTransform);

	executor->applyDamageInRadius(center, radius, damage, "Aelorin Nova");
}

void AelorinNovaState::finishAbility()
{
	if (m_completed)
	{
		return;
	}

	m_completed = true;

	const bool sent = AnimationAPI::sendTrigger(m_animation, "ToIdle");
	if (!sent)
	{
		Debug::warn("[AelorinNovaState] Failed to send ToIdle trigger");
	}
}

IMPLEMENT_SCRIPT(AelorinNovaState)
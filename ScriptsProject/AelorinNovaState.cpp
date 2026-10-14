#include "pch.h"
#include "AelorinNovaState.h"
#include "AelorinAttackExecutor.h"
#include "AelorinUI.h"
#include "AelorinVFX.h"
#include "CameraShake.h"

#include "AelorinAttackConfig.h"

AelorinNovaState::AelorinNovaState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void AelorinNovaState::OnStateEnter()
{
	Transform* parentTransform = TransformAPI::getParent(getOwner()->GetTransform());
	if (!parentTransform)
	{
		Debug::error("[AelorinNovaState] Aelorin transform not found.");
		return;
	}

	GameObject* parentGameObject = ComponentAPI::getOwner(parentTransform);
	GameObject* cameraObject = SceneAPI::getDefaultCameraGameObject();

	// get scripts
	m_controller = GameObjectAPI::findScript<AelorinBossController>(parentGameObject);
	m_animation = AnimationAPI::getAnimationComponent(getOwner());
	m_aelorinUI = GameObjectAPI::findScript<AelorinUI>(parentGameObject);
	m_vfx = GameObjectAPI::findScript<AelorinVFX>(parentGameObject);
	m_cameraShake = GameObjectAPI::findScript<CameraShake>(cameraObject);

	// reset members
	m_activeAbility = AelorinAbility::None;
	m_stateTimer = 0.0f;
	m_firstWaveApplied = false;
	m_completed = false;
	m_isFuryCast = false;

	if (!m_controller)
	{
		Debug::error("[AelorinNovaState] AelorinBossController not found.");
		return;
	}

	if (!m_animation)
	{
		Debug::error("[AelorinNovaState] AnimationComponent not found.");
		return;
	}

	if (!m_aelorinUI)
	{
		Debug::error("[AelorinNovaState] AelorinUI not found.");
	}

	if (m_controller->isPhase2())
	{
		AnimationAPI::setSpeedMultiplier(m_animation, 1.1f);
	}
	else
	{
		AnimationAPI::setSpeedMultiplier(m_animation, 1.2f);
	}

	// consume ability
	m_activeAbility = m_controller->consumeRequestedAbility();
	if (m_activeAbility != AelorinAbility::Nova)
	{
		Debug::warn("[AelorinNovaState] Unexpected requested ability!");
		return;
	}

	if (m_vfx)
	{
		if (m_controller->isPhase2())
		{
			m_vfx->startPhase2Spell();
		}
		else
		{
			m_vfx->startPhase1Spell();
		}
	}

	m_isFuryCast = m_controller->isFuryActive();

	if (m_controller->hasNovaCenterOverride())
	{
		m_novaCenter = m_controller->consumeNovaCenterOverride();
	}
	else
	{
		m_novaCenter = TransformAPI::getGlobalPosition(parentTransform);
	}

	if (m_aelorinUI)
	{
		const AelorinAttackConfig* config = m_controller->getAelorinAttackConfig();
		if (config)
		{
			m_aelorinUI->showNovaUI(
				m_novaCenter,
				config->m_novaRadius,
				config->m_novaChargeTime,
				false,
				config->m_novaRadius,
				0.0f
			);
		}
	}

	
	if (m_controller->isPhase2())
	{
		Transform* graspCenter = m_controller->getGraspCenter();
		if (graspCenter)
		{
			const Vector3 graspPosition = TransformAPI::getGlobalPosition(graspCenter);
			m_controller->facePositionInstant(graspPosition);
		}
	}
	else
	{
		const Vector3 lyrielPosition = m_controller->getLyrielPosition();
		const Vector3 deathPosition = m_controller->getDeathPosition();
		const Vector3 middlePosition = (lyrielPosition + deathPosition) * 0.5f;

		m_controller->facePositionInstant(middlePosition);
	}

	Debug::log("[AelorinNovaState] ENTER");
}

void AelorinNovaState::OnStateUpdate()
{
	if (!m_controller || !m_animation ||  m_completed)
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

	// Fury strips the nova windup + recovery REMOVED FURY
	const float chargeTime = config->m_novaChargeTime;
	const float recoveryDuration = config->m_novaRecoveryDuration;

	if (!m_firstWaveApplied && m_stateTimer >= chargeTime)
	{
		executeFirstNovaWave();
		m_firstWaveApplied = true;
	}

	if (m_stateTimer < chargeTime + recoveryDuration)
	{
		return;
	}

	if (m_vfx)
	{
		if (m_controller->isPhase2())
		{
			m_vfx->stopPhase2Spell();
		}
		else
		{
			m_vfx->stopPhase1Spell();
		}
	}

	finishAbility();
}

void AelorinNovaState::OnStateExit()
{
	if (m_aelorinUI)
	{
		m_aelorinUI->cancelNova();
	}

	if (m_vfx && m_controller)
	{
		m_vfx->stopPhase1Spell();
		m_vfx->stopPhase2Spell();
	}

	if (m_animation)
	{
		AnimationAPI::setSpeedMultiplier(m_animation, 1.0f);
	}

	m_aelorinUI = nullptr;
	m_vfx = nullptr;
	m_stateTimer = 0.0f;
	m_novaCenter = Vector3::Zero;
	m_firstWaveApplied = false;
	m_completed = false;
	m_isFuryCast = false;

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

	if (m_vfx)
	{
		m_vfx->playNovaImpactShockwave(m_novaCenter);
	}

	executeNovaWave(config->m_novaRadius, config->m_novaDamage);

	Debug::log("[AelorinNovaState] First Nova wave.");
}

void AelorinNovaState::executeNovaWave(float radius, float damage)
{
	AelorinAttackExecutor* executor = m_controller->getAttackExecutor();

	if (!executor)
	{
		return;
	}

	m_cameraShake->shake(0.04f, 0.25f);
	executor->applyDamageInRadius(m_novaCenter, radius, damage, "Aelorin Nova");
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
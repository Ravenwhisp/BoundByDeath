#include "pch.h"
#include "AelorinSpiritCannonState.h"

#include "AelorinAttackConfig.h"
#include "AelorinAttackExecutor.h"
#include "AelorinUI.h"
#include "AelorinVFX.h"

#include <cmath>
#include <cstdlib>

AelorinSpiritCannonState::AelorinSpiritCannonState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void AelorinSpiritCannonState::OnStateEnter()
{
	m_aelorinTransform = TransformAPI::getParent(getOwner()->GetTransform());
	if (!m_aelorinTransform)
	{
		Debug::error("[AelorinSpiritCannonState] Aelorin transform not found.");
		return;
	}

	GameObject* parentGameObject = ComponentAPI::getOwner(m_aelorinTransform);

	// get scripts
	m_controller = GameObjectAPI::findScript<AelorinBossController>(parentGameObject);
	m_animation = AnimationAPI::getAnimationComponent(getOwner());
	m_aelorinUI = GameObjectAPI::findScript<AelorinUI>(parentGameObject);
	m_vfx = GameObjectAPI::findScript<AelorinVFX>(parentGameObject);

	// reset members
	m_target = nullptr;
	m_lockedTargetPosition = Vector3::Zero;
	m_lockedAimDirection = Vector3::Zero;
	m_activeAbility = AelorinAbility::None;

	m_stateTimer = 0.0f;
	m_shotCount = 0;

	m_secondShotPrepared = false;
	m_completed = false;
	m_isFuryCast = false;

	if (!m_controller)
	{
		Debug::error("[AelorinSpiritCannonState] AelorinBossController not found.");
		return;
	}

	if (!m_animation)
	{
		Debug::error("[AelorinSpiritCannonState] AnimationComponent not found.");
		return;
	}

	if (!m_aelorinUI)
	{
		Debug::error("[AelorinSpiritCannonState] AelorinUI not found.");
	}

	m_attackExecutor = m_controller->getAttackExecutor();

	if (!m_attackExecutor)
	{
		Debug::error("[AelorinSpiritCannonState] AelorinAttackExecutor not found.");
		return;
	}

	// consume ability
	m_activeAbility = m_controller->consumeRequestedAbility();
	if (m_activeAbility != AelorinAbility::SpiritCannon)
	{
		Debug::warn("[AelorinSpiritCannonState] Unexpected requested ability!");
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

	beginShot();

	m_isFuryCast = m_controller->isFuryActive();
	if (m_isFuryCast)
	{
		m_controller->recordFuryCast();
	}

	Debug::log("[AelorinSpiritCannonState] ENTER");
}

void AelorinSpiritCannonState::OnStateUpdate()
{
	if (!m_controller || !m_attackExecutor || !m_animation || !m_aelorinTransform || m_completed)
	{
		return;
	}

	if (m_controller->trySendPriorityInterrupt(m_animation))
	{
		return;
	}

	const AelorinAttackConfig* config =	m_controller->getAelorinAttackConfig();
	if (!config)
	{
		return;
	}

	m_stateTimer += Time::getDeltaTime();

	const float shot1FireTime =	config->m_spiritCannonLockDuration;
	const float shot2PrepareTime = shot1FireTime + config->m_spiritCannonShotInterval;
	const float shot2FireTime =	shot2PrepareTime + config->m_spiritCannonLockDuration;
	const float finishTime = shot2FireTime +	config->m_spiritCannonRecoveryDuration;

	// Shot 1
	if (m_shotCount == 0 &&	m_stateTimer >= shot1FireTime)
	{
		fireShot();
		m_shotCount = 1;
	}

	// Prepare Shot 2
	if (m_shotCount == 1 &&	!m_secondShotPrepared && m_stateTimer >= shot2PrepareTime)
	{
		AnimationAPI::setPlaybackTime(m_animation, 0.0f);
		AnimationAPI::play(m_animation);
		beginShot();
		m_secondShotPrepared = true;
	}

	// Shot 2
	if (m_shotCount == 1 &&	m_secondShotPrepared &&	m_stateTimer >= shot2FireTime)
	{
		fireShot();
		m_shotCount = 2;
	}

	// Recovery complete
	if (m_shotCount >= 2 &&	m_stateTimer >= finishTime)
	{
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
}

void AelorinSpiritCannonState::OnStateExit()
{
	if (m_aelorinUI)
	{
		m_aelorinUI->cancelSpiritCannon();
	}

	if (m_controller)
	{
		m_controller->clearSpiritCannonDebugLine();
	}

	if (m_vfx && m_controller)
	{
		m_vfx->stopPhase1Spell();
		m_vfx->stopPhase2Spell();
	}

	m_controller = nullptr;
	m_attackExecutor = nullptr;
	m_animation = nullptr;
	m_aelorinUI = nullptr;
	m_vfx = nullptr;

	m_aelorinTransform = nullptr;
	m_target = nullptr;

	m_lockedTargetPosition = Vector3::Zero;
	m_lockedAimDirection = Vector3::Zero;

	m_activeAbility = AelorinAbility::None;

	m_stateTimer = 0.0f;
	m_shotCount = 0;

	m_secondShotPrepared = false;
	m_completed = false;
	m_isFuryCast = false;

	Debug::log("[AelorinSpiritCannonState] EXIT");
}

void AelorinSpiritCannonState::selectTarget()
{
	if (!m_controller)
	{
		return;
	}

	Transform* lyriel = m_controller->getLyrielTransform();
	Transform* death = m_controller->getDeathTransform();

	const bool lyrielValid = isValidTarget(lyriel);
	const bool deathValid = isValidTarget(death);

	if (lyrielValid && !deathValid)
	{
		m_target = lyriel;
		return;
	}

	if (!lyrielValid && deathValid)
	{
		m_target = death;
		return;
	}

	if (!lyrielValid && !deathValid)
	{
		m_target = nullptr;
		return;
	}

	m_target = (std::rand() % 2 == 0) ? lyriel : death; // Choose a random target if both are valid
}

bool AelorinSpiritCannonState::isValidTarget(Transform* targetTransform) const
{
	return m_attackExecutor && m_attackExecutor->isValidDamageTarget(targetTransform);
}

bool AelorinSpiritCannonState::lockCurrentTargetPosition()
{
	selectTarget();

	if (!m_target || !m_aelorinTransform)
	{
		return false;
	}

	const Vector3 origin = TransformAPI::getGlobalPosition(m_aelorinTransform);

	m_lockedTargetPosition = TransformAPI::getGlobalPosition(m_target);
	m_lockedAimDirection = m_lockedTargetPosition - origin;
	m_lockedAimDirection.y = 0.0f;

	if (m_lockedAimDirection.LengthSquared() <= 0.00001f)
	{
		m_lockedAimDirection = Vector3::Zero;
		return false;
	}

	m_lockedAimDirection.Normalize();
	if (m_controller)
	{
		m_controller->facePositionInstant(m_lockedTargetPosition);
	}

	return true;
}

void AelorinSpiritCannonState::beginShot()
{
	if (!m_controller)
	{
		return;
	}

	const AelorinAttackConfig* config =	m_controller->getAelorinAttackConfig();
	if (!config)
	{
		return;
	}

	if (!lockCurrentTargetPosition())
	{
		finishAbility();
		return;
	}

	if (m_aelorinUI)
	{
		m_aelorinUI->showSpiritCannonWarning(
			m_aelorinTransform,
			m_lockedAimDirection,
			config->m_spiritCannonBeamLength,
			config->m_spiritCannonTelegraphWidth,
			m_controller->isPhase2(),
			config->m_spiritCannonPhase2SideAngle,
			config->m_spiritCannonPhase2SideWidth
		);
	}

	Debug::log("[AelorinSpiritCannonState] Shot %d locked.", m_shotCount + 1);
}

void AelorinSpiritCannonState::fireShot()
{
	if (!m_controller || !m_attackExecutor || !m_aelorinTransform)
	{
		return;
	}

	const AelorinAttackConfig* config = m_controller->getAelorinAttackConfig();
	if (!config)
	{
		return;
	}

	if (m_lockedAimDirection.LengthSquared() <= 0.00001f)
	{
		return;
	}

	const Vector3 origin = TransformAPI::getGlobalPosition(m_aelorinTransform);
	const bool phase2 = m_controller->isPhase2();
	const float mainFireWidth = phase2 ? config->m_spiritCannonFireWidth * 1.20f : config->m_spiritCannonFireWidth;
	const float sideFireWidth = config->m_spiritCannonFireWidth * 0.70f;

	if (m_aelorinUI)
	{
		m_aelorinUI->fireSpiritCannonBeam(mainFireWidth, sideFireWidth, 0.20f);
	}

	m_attackExecutor->applyDamageInBeam(
		origin,
		m_lockedAimDirection,
		config->m_spiritCannonBeamLength,
		mainFireWidth,
		config->m_spiritCannonDamage,
		"Spirit Cannon"
	);

	// Phase 2
	if (phase2)
	{
		constexpr float degreesToRadians = 3.14159265f / 180.0f;

		const float angleRadians = config->m_spiritCannonPhase2SideAngle * degreesToRadians;
		const float cosAngle = std::cos(angleRadians);
		const float sinAngle = std::sin(angleRadians);

		const Vector3 leftDirection(
			m_lockedAimDirection.x * cosAngle - m_lockedAimDirection.z * sinAngle,
			0.0f,
			m_lockedAimDirection.x * sinAngle + m_lockedAimDirection.z * cosAngle
		);

		const Vector3 rightDirection(
			m_lockedAimDirection.x * cosAngle + m_lockedAimDirection.z * sinAngle,
			0.0f,
			m_lockedAimDirection.x * sinAngle + m_lockedAimDirection.z * cosAngle
		);

		m_attackExecutor->applyDamageInBeam(
			origin,
			leftDirection,
			config->m_spiritCannonBeamLength,
			sideFireWidth,
			config->m_spiritCannonDamage,
			"Spirit Cannon"
		);

		m_attackExecutor->applyDamageInBeam(
			origin,
			rightDirection,
			config->m_spiritCannonBeamLength,
			sideFireWidth,
			config->m_spiritCannonDamage,
			"Spirit Cannon"
		);
	}
}

void AelorinSpiritCannonState::finishAbility()
{
	if (m_completed)
	{
		return;
	}

	m_completed = true;

	if (!AnimationAPI::sendTrigger(m_animation,	"ToIdle"))
	{
		Debug::warn("[AelorinSpiritCannonState] Failed to send ToIdle.");
	}
}

IMPLEMENT_SCRIPT(AelorinSpiritCannonState)
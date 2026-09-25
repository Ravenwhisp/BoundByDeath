#include "pch.h"
#include "AelorinSpiritCannonState.h"

#include "AelorinAttackConfig.h"
#include "AelorinAttackExecutor.h"
#include "AelorinUI.h"

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

	// reset members
	m_target = nullptr;
	m_lockedTargetPosition = Vector3::Zero;
	m_lockedAimDirection = Vector3::Zero;
	m_activeAbility = AelorinAbility::None;

	m_shotTimer = 0.0f;
	m_intervalTimer = 0.0f;
	m_recoveryTimer = 0.0f;

	m_shotCount = 0;

	m_shotActive = false;
	m_waitingForNextShot = false;
	m_recovering = false;
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

	const float deltaTime =	Time::getDeltaTime();

	// Locked Telegraph

	if (m_shotActive)
	{
		m_shotTimer += deltaTime;

		if (m_shotTimer < config->m_spiritCannonLockDuration)
		{
			return;
		}

		fireShot();

		m_shotActive = false;

		++m_shotCount;

		// Two Shots
		if (m_shotCount >= 2)
		{
			m_recovering = true;
			m_recoveryTimer = 0.0f;
			return;
		}

		m_waitingForNextShot = true;
		m_intervalTimer = 0.0f;

		return;
	}

	// Between Shots

	if (m_waitingForNextShot)
	{
		m_intervalTimer += deltaTime;

		if (m_intervalTimer >= config->m_spiritCannonShotInterval)
		{
			beginShot();
		}

		return;
	}

	// Recovery

	if (m_recovering)
	{
		m_recoveryTimer += deltaTime;

		if (m_recoveryTimer >= config->m_spiritCannonRecoveryDuration)
		{
			finishAbility();
		}
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

	m_controller = nullptr;
	m_attackExecutor = nullptr;
	m_animation = nullptr;
	m_aelorinUI = nullptr;

	m_aelorinTransform = nullptr;
	m_target = nullptr;

	m_lockedTargetPosition = Vector3::Zero;
	m_lockedAimDirection = Vector3::Zero;

	m_activeAbility = AelorinAbility::None;

	m_shotTimer = 0.0f;
	m_intervalTimer = 0.0f;
	m_recoveryTimer = 0.0f;

	m_shotCount = 0;

	m_shotActive = false;
	m_waitingForNextShot = false;
	m_recovering = false;
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

	return true;
}

void AelorinSpiritCannonState::beginShot()
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

	if (!lockCurrentTargetPosition())
	{
		finishAbility();
		return;
	}

	m_shotTimer = 0.0f;
	m_shotActive = true;

	m_waitingForNextShot = false;
	m_intervalTimer = 0.0f;

	if (m_aelorinUI)
	{
		m_aelorinUI->showSpiritCannonWarning(
			m_aelorinTransform,
			m_lockedAimDirection,
			config->m_spiritCannonBeamLength,
			config->m_spiritCannonTelegraphWidth
		);
	}

	Debug::log("[AelorinSpiritCannonV2State] Shot %d locked.", m_shotCount + 1);
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
	m_controller->setSpiritCannonDebugLine(origin, m_lockedAimDirection, config->m_spiritCannonFireWidth);

	if (m_aelorinUI)
	{
		m_aelorinUI->fireSpiritCannonBeam(config->m_spiritCannonFireWidth, 0.20f);
	}

	m_attackExecutor->applyDamageInBeam(
		origin,
		m_lockedAimDirection,
		config->m_spiritCannonBeamLength,
		config->m_spiritCannonFireWidth,
		config->m_spiritCannonDamage,
		"Spirit Cannon"
	);
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
		Debug::warn("[AelorinSpiritCannonV2State] Failed to send ToIdle.");
	}
}

IMPLEMENT_SCRIPT(AelorinSpiritCannonState)
#include "pch.h"
#include "AelorinRisenSpiresState.h"

#include "AelorinAttackConfig.h"
#include "AelorinAttackExecutor.h"
#include "AelorinUI.h"
#include "AelorinVFX.h"
#include "CameraShake.h"

#include <cstdlib> // for random

#define ARENA_RADIUS 11.6f

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
	m_firstPassExecuted = false;
	m_secondPassExecuted = false;
	m_secondPassUIShown = false;
	m_completed = false;
	m_isFuryCast = false;

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

	if (!m_aelorinUI)
	{
		Debug::error("[AelorinRisenSpiresState] AelorinUI not found.");
	}

	if (m_controller->isPhase2())
	{
		AnimationAPI::setSpeedMultiplier(m_animation, 1.5f);
	}
	else
	{
		AnimationAPI::setSpeedMultiplier(m_animation, 1.0f);
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
	if (m_isFuryCast)
	{
		m_controller->recordFuryCast();
	}

	if (m_aelorinUI)
	{
		const AelorinAttackConfig* config =	m_controller->getAelorinAttackConfig();
		if (config)
		{
			Transform* patternA = m_controller->getRisenSpiresPatternARoot();

			executePattern(patternA, "Risen Spires Pass 1", false);
			playCenterVFXForPattern(patternA);
			m_aelorinUI->showRisenSpiresUI(patternA, config->m_risenSpiresRadius, config->m_risenSpiresWindupDuration);

			executePattern(m_controller->getRisenSpiresPatternBRoot(), "Risen Spires Pass 2", false);
		}
	}

	const Vector3 lyrielPosition = m_controller->getLyrielPosition();
	const Vector3 deathPosition = m_controller->getDeathPosition();

	const Vector3 middlePosition = (lyrielPosition + deathPosition) * 0.5f;

	m_controller->facePositionInstant(middlePosition);

	Debug::log("[AelorinRisenSpiresState] ENTER");
}

void AelorinRisenSpiresState::OnStateUpdate()
{
	if (!m_controller || !m_attackExecutor || !m_animation || m_completed)
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

	// Pick Fury type cast or Normal
	const float windupDuration = m_isFuryCast ? 0.0f : config->m_risenSpiresWindupDuration;
	const float recoveryDuration = m_isFuryCast ? 0.0f : config->m_risenSpiresRecoveryDuration;

	// First pass after the 3 second windup
	if (!m_firstPassExecuted &&	m_stateTimer >= windupDuration)
	{
		if (m_aelorinUI)
		{
			m_aelorinUI->fireRisenSpiresBeams(0.06f);
		}

		executePattern(m_controller->getRisenSpiresPatternARoot(), "Risen Spires Pass 1", true);

		m_firstPassExecuted = true;
	}

	constexpr float risenSpiresBeamDuration = 0.06f;
	constexpr float secondPassUIRevealDelay = 0.07f;

	if (m_controller->isPhase2() && m_firstPassExecuted && !m_secondPassUIShown && m_stateTimer >= windupDuration + secondPassUIRevealDelay)
	{
		if (m_aelorinUI)
		{
			Transform* patternB = m_controller->getRisenSpiresPatternBRoot();

			const float remainingWarningDuration = (std::max)(config->m_risenSpiresPhase2SecondPassDelay - secondPassUIRevealDelay, 0.001f);

			playCenterVFXForPattern(patternB);

			m_aelorinUI->showRisenSpiresUI(patternB, config->m_risenSpiresRadius, remainingWarningDuration);
		}

		m_secondPassUIShown = true;
	}

	// Phase 2 gets opposite pattern 2 seconds later
	if (m_controller->isPhase2() && m_firstPassExecuted && !m_secondPassExecuted && m_stateTimer >= windupDuration + config->m_risenSpiresPhase2SecondPassDelay)
	{
		if (m_aelorinUI)
		{
			m_aelorinUI->fireRisenSpiresBeams(0.06f);
		}

		executePattern(m_controller->getRisenSpiresPatternBRoot(), "Risen Spires Pass 2", true);

		m_secondPassExecuted = true;
	}

	const float lastPassTime = m_controller->isPhase2() ? windupDuration + config->m_risenSpiresPhase2SecondPassDelay : windupDuration;

	if (m_stateTimer < lastPassTime + recoveryDuration)
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

void AelorinRisenSpiresState::OnStateExit()
{
	if (m_aelorinUI)
	{
		m_aelorinUI->cancelRisenSpires();
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
	m_cameraShake = nullptr;
	m_stateTimer = 0.0f;
	m_firstPassExecuted = false;
	m_secondPassExecuted = false;
	m_secondPassUIShown = false;
	m_completed = false;
	m_isFuryCast = false;

	Debug::log("[AelorinRisenSpiresState] EXIT");
}

void AelorinRisenSpiresState::executePattern(Transform* patternRoot, const char* sourceName, bool dealDamage)
{
	if (!patternRoot || !m_attackExecutor)
	{
		return;
	}

	const AelorinAttackConfig* config =	m_controller->getAelorinAttackConfig();
	if (!config)
	{
		return;
	}

	const int childCount = TransformAPI::getChildCount(patternRoot);

	// EXECUTE
	if (dealDamage)
	{
		for (int i = 0; i < childCount; ++i)
		{
			Transform* spirePoint = TransformAPI::getChild(patternRoot,	i);
			if (!spirePoint)
			{
				continue;
			}

			const Vector3 position = TransformAPI::getGlobalPosition(spirePoint);

			m_vfx->playRisenSpiresImpact(position);
			m_attackExecutor->applyDamageInRadius(position, config->m_risenSpiresRadius, config->m_risenSpiresDamage, sourceName);

		}

		if (m_cameraShake)
		{
			m_cameraShake->shake(0.04f, 0.25f);
		}

		Debug::log("[AelorinRisenSpiresState] Executed pattern with %d spires", childCount);

		return;
	}

	// Generate the positions before the telegraph appears
	std::vector<Vector2> spirePositions = generateSpirePositions(childCount, ARENA_RADIUS, config->m_risenSpiresRadius * 2.1f, config);

	Vector3 arenaCenter = Vector3::Zero;
	Transform* centerTransform = m_controller->getGraspCenter();
	if (centerTransform)
	{
		arenaCenter = TransformAPI::getGlobalPosition(centerTransform);
	}

	for (int i = 0; i < childCount; ++i)
	{
		Transform* spirePoint = TransformAPI::getChild(patternRoot, i);
		if (!spirePoint)
		{
			continue;
		}

		const Vector2& spirePosition = spirePositions.at(i);

		TransformAPI::setGlobalPosition(spirePoint, Vector3(spirePosition.x, arenaCenter.y, spirePosition.y));
	}

	Debug::log("[AelorinRisenSpiresState] Prepared pattern with %d spires", childCount);
}

void AelorinRisenSpiresState::playCenterVFXForPattern(Transform* patternRoot)
{
	if (!patternRoot || !m_vfx)
	{
		return;
	}
	
	const int childCount = TransformAPI::getChildCount(patternRoot);

	for (int i = 0; i < childCount; ++i)
	{
		Transform* spirePoint =	TransformAPI::getChild(patternRoot, i);
		if (!spirePoint)
		{
			continue;
		}

		const Vector3 position = TransformAPI::getGlobalPosition(spirePoint);

		m_vfx->playRisenSpiresImpactCenter(position);
	}
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

std::vector<Vector2> AelorinRisenSpiresState::generateSpirePositions(
	int count,
	float arenaRadius,
	float minDistance,
	const AelorinAttackConfig* config)
{
	std::vector<Vector2> positions;

	constexpr int MAX_ATTEMPTS = 100;

	while (positions.size() < count)
	{
		bool found = false;

		for (int attempt = 0; attempt < MAX_ATTEMPTS; ++attempt)
		{
			Vector2 candidate = randomPointInArena(config);

			bool valid = true;

			for (const Vector2& p : positions)
			{
				if (Vector2::DistanceSquared(candidate, p) <
					minDistance * minDistance)
				{
					valid = false;
					break;
				}
			}

			if (valid)
			{
				positions.push_back(candidate);
				found = true;
				break;
			}
		}

		if (!found)
			positions.push_back(Vector2(200, 200));
	}

	return positions;
}

Vector2 AelorinRisenSpiresState::randomPointInArena(const AelorinAttackConfig* config) {
	Vector3 arenaCenter = Vector3::Zero;
	Transform* centerTransform = m_controller->getGraspCenter();
	if (centerTransform)
	{
		arenaCenter = TransformAPI::getGlobalPosition(centerTransform);
	}
	float eligibleSpawnRadius = ARENA_RADIUS - config->m_risenSpiresRadius;
	float xRandDisplacement = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * eligibleSpawnRadius * 2 - eligibleSpawnRadius;
	float zRandDisplacement = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * eligibleSpawnRadius * 2 - eligibleSpawnRadius;
	return std::move(Vector2(arenaCenter.x + xRandDisplacement, arenaCenter.z + zRandDisplacement));
}

IMPLEMENT_SCRIPT(AelorinRisenSpiresState)
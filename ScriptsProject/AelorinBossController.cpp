#include "pch.h"
#include "AelorinBossController.h"

#include "AelorinDetectionAggro.h"
#include "AelorinDamageable.h"
#include "AelorinAttackConfig.h"

#include "HealthDropSpawner.h"

IMPLEMENT_SCRIPT_FIELDS_INHERITED(AelorinBossController, EnemyBaseController,
	SERIALIZED_ASSET_REF(m_attackConfig, "Attack Config", AssetType::DATA_CONTAINER)
)

AelorinBossController::AelorinBossController(GameObject* owner) : EnemyBaseController(owner)
{
}

void AelorinBossController::Start()
{
	EnemyBaseController::Start();

	m_aelorinDetectionAggro = GameObjectAPI::findScript<AelorinDetectionAggro>(getOwner());
	m_damageable = GameObjectAPI::findScript<AelorinDamageable>(getOwner());

	Transform* phase1Model = TransformAPI::findChildByName(getOwner()->GetTransform(), "Phase1");
	if (!phase1Model)
	{
		Debug::error("[AelorinBossController] Phase 1 Model not found!");
		return;
	}	

	Transform* phase2Model = TransformAPI::findChildByName(getOwner()->GetTransform(), "Phase2");
	if (!phase2Model)
	{
		Debug::error("[AelorinBossController] Phase 2 Model not found!");
		return;
	}

	m_phase1GameObject = ComponentAPI::getOwner(phase1Model);
	m_phase2GameObject = ComponentAPI::getOwner(phase2Model);

	if (!m_aelorinDetectionAggro)
	{
		Debug::error("[AelorinBossController] AelorinDetectionAggro script not found!");
	}

	if (!m_damageable)
	{
		Debug::error("[AelorinBossController] AelorinDamageable script not found!");
	}

	if (!m_phase1GameObject)
	{
		Debug::error("[AelorinBossController] Phase 1 Game Object not found!");
	}

	if (!m_phase2GameObject)
	{
		Debug::error("[AelorinBossController] Phase 2 Game Object not found!");
	}
}

//void AelorinBossController::drawGizmo()
//{
//
//}

void AelorinBossController::Update()
{
	updateEncounter();
}

void AelorinBossController::updateEncounter()
{
	if (!m_hasStartedEncounter && m_aelorinDetectionAggro)
	{
		if (m_aelorinDetectionAggro->startEncounter())
		{
			m_hasStartedEncounter = true;
		}
	}
}

float AelorinBossController::getDecisionTime() const
{
	const AelorinAttackConfig* config = m_attackConfig.get();

	if (!config)
	{
		return isPhase2() ? 1.0f : 2.0f;
	}

	return isPhase2() ? config->m_phase2DecisionTime : config->m_phase1DecisionTime;
}

void AelorinBossController::setPhase(Phase phase)
{
	m_phase = phase;
}

void AelorinBossController::requestPhaseTransition()
{
	if (isPhase2() || m_phaseTransitionRequested)
	{
		return;
	}

	m_phaseTransitionRequested = true;

	Debug::log("[AelorinBossController] Phase transition requested.");
}

void AelorinBossController::markPhaseTransitionTriggered()
{
	m_phaseTransitionTriggered = true;
}

void AelorinBossController::beginPhase2()
{
	if (isPhase2())
	{
		return;
	}

	if (!m_phase1GameObject || !m_phase2GameObject)
	{
		return;
	}

	GameObjectAPI::setActive(m_phase1GameObject, false);
	GameObjectAPI::setActive(m_phase2GameObject, true);

	setPhase(Phase::Phase2);

	m_phaseTransitionRequested = false;
	m_phaseTransitionTriggered = false;

	if (m_damageable)
	{
		m_damageable->beginPhase2();
	}

	Debug::log("[AelorinBossController] Phase 2 started.");
}

bool AelorinBossController::trySendPhaseTransitionTrigger(AnimationComponent* animation)
{
	if (!m_phaseTransitionRequested)
	{
		return false;
	}

	if (m_phaseTransitionTriggered)
	{
		return false;
	}

	if (!animation)
	{
		return false;
	}

	const bool sent = AnimationAPI::sendTrigger(animation, "ToPhaseTransition");

	if (!sent)
	{
		return false;
	}

	markPhaseTransitionTriggered();

	Debug::log("[AelorinBossController] ToPhaseTransition trigger sent.");

	return true;
}

void AelorinBossController::requestThresholdStagger()
{
	if (m_thresholdStaggerRequested)
	{
		return;
	}

	m_thresholdStaggerRequested = true;
	m_thresholdStaggerTriggered = false;

	Debug::log("[AelorinBossController] Threshold stagger requested.");
}

bool AelorinBossController::trySendThresholdStaggerTrigger(AnimationComponent* animation)
{
	if (!m_thresholdStaggerRequested)
	{
		return false;
	}

	if (m_thresholdStaggerTriggered)
	{
		return false;
	}

	if (!animation)
	{
		return false;
	}

	const bool sent = AnimationAPI::sendTrigger(animation, "ToThresholdStagger");

	if (!sent)
	{
		return false;
	}

	m_thresholdStaggerTriggered = true;

	Debug::log("[AelorinBossController] ToThresholdStagger trigger sent.");

	return true;
}

void AelorinBossController::completeThresholdStagger()
{
	m_thresholdStaggerRequested = false;
	m_thresholdStaggerTriggered = false;

	Debug::log("[AelorinBossController] Threshold stagger completed.");
}

float AelorinBossController::getThresholdStaggerDuration() const
{
	const AelorinAttackConfig* config = m_attackConfig.get();

	if (!config)
	{
		return 2.0f;
	}

	return config->m_thresholdStaggerDuration;
}

void AelorinBossController::spawnHealthDrops()
{
	const AelorinAttackConfig* config = m_attackConfig.get();

	if (!config)
	{
		Debug::warn("[AelorinBossController] Cannot spawn health drops: AelorinAttackConfig is missing.");
		return;
	}

	if (!config->m_healthPickupPrefab.m_id.isValid())
	{
		Debug::warn("[AelorinBossController] Cannot spawn health drops: health pickup prefab is invalid.");
		return;
	}

	Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
	if (!ownerTransform)
	{
		Debug::warn("[AelorinBossController] Cannot spawn health drops: owner transform is missing.");
		return;
	}

	const Vector3 position = TransformAPI::getGlobalPosition(ownerTransform);

	for (int i = 0; i < config->m_healthDropQuantity; ++i)
	{
		HealthDropSpawner::drop(
			config->m_healthPickupPrefab.m_id,
			position,
			config->m_healingAmount,
			config->m_dropRadius,
			config->m_dropHeight
		);
	}
}

// These two will not be needed
Transform* AelorinBossController::acquireCurrentTarget()
{
	return nullptr;
}

bool AelorinBossController::isTargetDowned(Transform* target) const
{
	return false;
}
// -----------------------------

IMPLEMENT_SCRIPT(AelorinBossController)
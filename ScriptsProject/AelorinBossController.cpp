#include "pch.h"
#include "AelorinBossController.h"

#include "AelorinDetectionAggro.h"
#include "AelorinDamageable.h"

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
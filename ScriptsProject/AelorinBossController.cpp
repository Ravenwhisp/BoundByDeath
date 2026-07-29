#include "pch.h"
#include "AelorinBossController.h"

#include "AelorinDetectionAggro.h"
#include "EnemyDamageable.h"

AelorinBossController::AelorinBossController(GameObject* owner) : EnemyBaseController(owner)
{
}

void AelorinBossController::Start()
{
	EnemyBaseController::Start();

	m_aelorinDetectionAggro = GameObjectAPI::findScript<AelorinDetectionAggro>(getOwner());
	m_damageable = GameObjectAPI::findScript<EnemyDamageable>(getOwner());

	Transform* phase1Model = TransformAPI::findChildByName(getOwner()->GetTransform(), "Phase1");
	m_phase1GameObject = ComponentAPI::getOwner(phase1Model);
	Transform* phase2Model = TransformAPI::findChildByName(getOwner()->GetTransform(), "Phase2");
	m_phase2GameObject = ComponentAPI::getOwner(phase2Model);

	if (!m_aelorinDetectionAggro)
	{
		Debug::error("[AelorinBossController] AelorinDetectionAggro script not found!");
	}

	if (!m_damageable)
	{
		Debug::error("[AelorinBossController] EnemyDamageable script not found!");
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
	if (!m_hasStartedEncounter && m_aelorinDetectionAggro)
	{
		if (m_aelorinDetectionAggro->startEncounter())
		{
			m_hasStartedEncounter = true;
		}
	}

	updateBossPhase();
}

void AelorinBossController::setPhase(Phase phase)
{
	m_phase = phase;
}

void AelorinBossController::updateBossPhase()
{
	if (isPhase2())
	{
		return;
	}

	if (!m_aelorinDetectionAggro || !m_hasStartedEncounter)
	{
		return;
	}

	if (!m_damageable)
	{
		return;
	}

	if (m_damageable->getMaxHp() <= 0.0f)
	{
		return;
	}

	if (m_damageable->getCurrentHp() <= 0.0f)
	{
		setPhase(Phase::Phase2);
		Debug::log("Phase 2 has started!");
		GameObjectAPI::setActive(m_phase1GameObject, false);
		GameObjectAPI::setActive(m_phase2GameObject, true);
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
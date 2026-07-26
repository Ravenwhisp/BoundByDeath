#include "pch.h"
#include "CheckpointEvent.h"

#include "ReaperGauge.h"
#include "Bound.h"
#include "Damageable.h"
#include "PersistingPowerupState.h"

CheckpointEvent::CheckpointEvent(GameObject* owner)
    : GameplayEventAction(owner)
{
}

void CheckpointEvent::Start()
{
	auto managers = SceneAPI::findAllGameObjectsWithScript<ReaperGauge>();
	GameObject* manager = nullptr;
	for (GameObject* obj : managers)
	{
		m_reaperGauge = GameObjectAPI::findScript<ReaperGauge>(obj);
		if (m_reaperGauge)
		{
			manager = obj;
			break;
		}
	}
	if (manager)
	{
		Bound* boundScript = GameObjectAPI::findScript<Bound>(manager);
		m_lyrielDamageable = boundScript ? boundScript->m_firstDamageable : nullptr;
		m_deathDamageable = boundScript ? boundScript->m_secondDamageable : nullptr;
	}

	m_checkpointManager = &CheckpointManager::Get();

	if (!m_checkpointManager)
	{
		Debug::warn("CheckpointEvent: CheckpointManager singleton not found.");
		return;
	}

	if (!m_reaperGauge)
	{
		Debug::warn("CheckpointSetup: ReaperGauge script not found in scene.");
		return;
	}
}

void CheckpointEvent::Update()
{
}

void CheckpointEvent::executeEvent(GameplayEventTrigger* trigger)
{
	if(m_checkpointManager)
	{
		if(m_checkpointManager->GetLastCheckpoint() >= m_checkpointId)
		{
			Debug::log("CheckpointEvent: Checkpoint %d already saved, skipping.", static_cast<int>(m_checkpointId));
			return;
		}
		bool* currentPowerups = PersistingPowerupState::getUnlockedPowerupState();

		CheckpointManager* checkpointManager = &CheckpointManager::Get();

		checkpointManager->m_savedLyrielHealth = m_lyrielDamageable ? m_lyrielDamageable->getCurrentHp() : 0.0f;
		checkpointManager->m_savedDeathHealth = m_deathDamageable ? m_deathDamageable->getCurrentHp() : 0.0f;
		checkpointManager->m_savedReaperGaugeAmount = m_reaperGauge ? m_reaperGauge->getGauge() : 0.0f;

		std::copy(currentPowerups,
			currentPowerups + static_cast<int>(PowerupId::Count),
			checkpointManager->m_savedUnlockedPowerups);

		m_checkpointManager->SetCheckpoint(m_checkpointId);
		Debug::log("CheckpointEvent: Checkpoint %d saved.", static_cast<int>(m_checkpointId));
	}
}

IMPLEMENT_SCRIPT(CheckpointEvent)
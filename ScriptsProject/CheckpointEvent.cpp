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
	auto managers = SceneAPI::findAllGameObjectsWithScript<CheckpointManager>();
	GameObject* manager = nullptr;
	for (GameObject* obj : managers)
	{
		m_checkpointManager = GameObjectAPI::findScript<CheckpointManager>(obj);
		if (m_checkpointManager)
		{
			manager = obj;
			break;
		}
	}
	if (manager)
	{
		m_reaperGauge = GameObjectAPI::findScript<ReaperGauge>(manager);
		Bound* boundScript = GameObjectAPI::findScript<Bound>(manager);
		m_lyrielDamageable = boundScript ? boundScript->m_firstDamageable : nullptr;
		m_deathDamageable = boundScript ? boundScript->m_secondDamageable : nullptr;
	}

	if (!m_checkpointManager)
	{
		Debug::warn("CheckpointSetup: CheckpointManager script not found in scene.");
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
		CharacterCheckpointState state;
		state.m_lyrielHealth = m_lyrielDamageable ? m_lyrielDamageable->getCurrentHp() : 0.0f;
		state.m_deathHealth = m_deathDamageable ? m_deathDamageable->getCurrentHp() : 0.0f;
		state.m_reaperGaugeAmount = m_reaperGauge ? m_reaperGauge->getGauge() : 0.0f;
		
		bool* currentPowerups = PersistingPowerupState::getUnlockedPowerupState();

		std::copy(currentPowerups, currentPowerups + static_cast<int>(PowerupId::Count), state.m_unlockedPowerups);

		m_checkpointManager->SaveState(m_owner->GetID(), state);

		m_checkpointManager->SetCheckpointId(m_checkpointId);
	}
}

IMPLEMENT_SCRIPT(CheckpointEvent)
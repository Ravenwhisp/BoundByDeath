#include "pch.h"
#include "CheckpointManager.h"

#include "CheckpointSetup.h"
#include "EnemyDamageable.h"
#include "BreakableObject.h"
#include "PowerupCollectible.h"

CheckpointManager& CheckpointManager::Get() 
{
	static CheckpointManager instance;
	return instance;
}

void CheckpointManager::SetCheckpoint(CheckpointId checkpointId)
{
	m_lastCheckpointId = checkpointId;

	// Save active states of dynamic objects in the current checkpoint
	// Character states not needed since they are saved in the CheckpointEvent script
	m_enemyActiveStates = m_enemyStates;
	m_breakableActiveStates = m_breakableStates;
	m_collectibleActiveStates = m_collectibleStates;
}

void CheckpointManager::ApplyCheckpoint()
{
	//cuando la escena se ha reseteado, y hay checkpoint guardado, recorremos los maps y aplicamos el estado guardado a cada objeto correspondiente

	if(m_lastCheckpointId == CheckpointId::NONE)
	{
		return;
	}

	// 1. Apply hardcoded checkpoints
	LoadCheckpointByID();

	// 2. Apply saved states for dynamic objects
	
	for(const auto& [id, state] : m_characterStates)
	{
		GameObject* characterGO = FindGameObject(id);
		if(characterGO)
		{
			Damageable* damageable = GameObjectAPI::findScript<Damageable>(characterGO);
			if (damageable)
			{
				
			}
		}
	}

	for (const auto& [id, state] : m_enemyActiveStates)
	{
		GameObject* enemyGO = FindGameObject(id);
		if(enemyGO)
		{
			EnemyDamageable* damageable = GameObjectAPI::findScript<EnemyDamageable>(enemyGO);
			if (damageable && state.m_isDead)
			{
				damageable->kill();
			}
		}
	}

	for (const auto& [id, state] : m_breakableActiveStates)
	{
		GameObject* breakableGO = FindGameObject(id);
		if(breakableGO)
		{
			BreakableObject* breakable = GameObjectAPI::findScript<BreakableObject>(breakableGO);
			if (breakable && state.m_isBroken)
			{
				breakable->onBreak();
			}
		}
	}

	for (const auto& [id, state] : m_collectibleActiveStates)
	{
		GameObject* collectibleGO = FindGameObject(id);
		if(collectibleGO)
		{
			if (state.m_isCollected)
			{
				GameObjectAPI::removeGameObject(collectibleGO);
			}
		}
	}
}

void CheckpointManager::LoadCheckpointByID()
{
	for (auto const& [id, setup] : m_checkpointSetupPool)
	{
		if(id > m_lastCheckpointId)
		{
			break;
		}

		if(setup != nullptr)
		{
			setup->ApplyCheckpointState();
		}
	}
}
#include "pch.h"
#include "CheckpointManager.h"

#include "CheckpointSetup.h"

CheckpointManager& CheckpointManager::Get() 
{
	static CheckpointManager instance;
	return instance;
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
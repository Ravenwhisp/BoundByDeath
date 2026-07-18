#include "pch.h"
#include "CheckpointSetup.h"

#include "CheckpointManager.h"

CheckpointSetup::CheckpointSetup(GameObject* owner)
    : Script(owner)
{
}

void CheckpointSetup::Start()
{
	m_checkpointManager = &CheckpointManager::Get();

	if(!m_checkpointManager)
	{
		Debug::warn("CheckpointSetup: CheckpointManager singleton not found.");
		return;
	}

	m_checkpointManager->RegisterCheckpoint(m_checkpointId, this);
}


IMPLEMENT_SCRIPT(CheckpointSetup)
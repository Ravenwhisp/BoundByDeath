#include "pch.h"
#include "CheckpointSetup.h"

#include "CheckpointManager.h"

CheckpointSetup::CheckpointSetup(GameObject* owner)
    : Script(owner)
{
}

void CheckpointSetup::Start()
{
	auto managers = SceneAPI::findAllGameObjectsWithScript<CheckpointManager>();
	for (GameObject* obj : managers)
	{
		m_checkpointManager = GameObjectAPI::findScript<CheckpointManager>(obj);
		if (m_checkpointManager) break;
	}

	if (!m_checkpointManager)
	{
		Debug::warn("CheckpointSetup: CheckpointManager script not found in scene.");
		return;
	}

	m_checkpointManager->RegisterCheckpoint(m_checkpointId, this);
}


IMPLEMENT_SCRIPT(CheckpointSetup)
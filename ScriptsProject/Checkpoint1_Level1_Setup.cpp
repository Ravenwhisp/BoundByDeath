#include "pch.h"
#include "Checkpoint1_Level1_Setup.h"

#include "PuzzleManagerLVL1.h"

Checkpoint1_Level1_Setup::Checkpoint1_Level1_Setup(GameObject* owner)
    : CheckpointSetup(owner)
{
}

void Checkpoint1_Level1_Setup::Start()
{
    CheckpointSetup::Start();

	auto managers = SceneAPI::findAllGameObjectsWithScript<PuzzleManagerLVL1>();
	for (GameObject* obj : managers)
	{
		m_puzzleManager = GameObjectAPI::findScript<PuzzleManagerLVL1>(obj);
		if (m_puzzleManager) break;
	}

	if (!m_puzzleManager)
	{
		Debug::warn("Checkpoint1_Level1_Setup: PuzzleManagerLVL1 script not found in scene.");
		return;
	}
}

void Checkpoint1_Level1_Setup::ApplyCheckpointState()
{
	//Llamamos a CheckpointSetup::ApplyCheckpointState() para aplicar el lugar de spawn de Lyriel y Death
	CheckpointSetup::ApplyCheckpointState();

	// Checkpoint 1: Completar puzzle cristales
	if (m_puzzleManager)
	{
		// For this checkpoint, we want to ensure that the first puzzle is marked as solved
		m_puzzleManager->puzzle1Solved();
	}
	else
	{
		Debug::warn("Checkpoint1_Level1_Setup: Cannot apply checkpoint state because PuzzleManagerLVL1 is not found.");
	}
}

IMPLEMENT_SCRIPT(Checkpoint1_Level1_Setup)
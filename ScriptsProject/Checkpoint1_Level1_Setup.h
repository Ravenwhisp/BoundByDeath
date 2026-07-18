#pragma once

#include "ScriptAPI.h"

#include "CheckpointSetup.h"

class PuzzleManagerLVL1;

class Checkpoint1_Level1_Setup : public CheckpointSetup
{
    DECLARE_SCRIPT(Checkpoint1_Level1_Setup)

public:
    explicit Checkpoint1_Level1_Setup(GameObject* owner);

	void Start() override;	

	void ApplyCheckpointState() override;

private:
	PuzzleManagerLVL1* m_puzzleManager = nullptr;
	CheckpointId m_checkpointId = CheckpointId::CHECKPOINT_1_LEVEL_1;
};
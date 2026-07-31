#pragma once

#include "ScriptAPI.h"
#include "PersistingPowerupState.h"
#include <map>
#include <array>

enum CheckpointId
{
	NONE = 0,

    CHECKPOINT_1_LEVEL_1= 100,
    CHECKPOINT_2_LEVEL_1,
    CHECKPOINT_3_LEVEL_1,

	CHECKPOINT_1_LEVEL_2 = 200,

	CHECKPOINT_1_LEVEL_3 = 300,
	//... Add more checkpoints as needed
};

enum PuzzleId
{
    PUZZLE1_LEVEL1 = 0,
    PUZZLE2_LEVEL1,
    PUZZLE3_LEVEL1,
    COUNT
    //... Add more puzzle ids as needed
};

class PersistingCheckpointState
{
public:
    static PersistingCheckpointState& Get();

    void SetCheckpoint(CheckpointId checkpointId);

    void Reset();

public:
    CheckpointId m_lastCheckpointId = CheckpointId::NONE;

    float m_savedReaperGaugeAmount = 0.f;
    bool m_savedUnlockedPowerups[static_cast<int>(PowerupId::Count)];

    float m_savedLyrielHealth = 0.f;
    float m_savedDeathHealth = 0.f;

    Vector3 m_savedLyrielRespawn;
    Vector3 m_savedDeathRespawn;

    std::vector<UID> m_deadEnemies;
    std::vector<UID> m_brokenBreakables;
    std::vector<UID> m_triggeredEvents;
    std::array<bool, static_cast<size_t>(PuzzleId::COUNT)> m_solvedPuzzles{};

    std::vector<UID> m_deadEnemiesPersistent;
    std::vector<UID> m_brokenBreakablesPersistent;
    std::vector<UID> m_triggeredEventsPersistent;
    std::array<bool, static_cast<size_t>(PuzzleId::COUNT)> m_solvedPuzzlesPersistent{};

};
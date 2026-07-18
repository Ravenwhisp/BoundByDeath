#pragma once

#include "ScriptAPI.h"
#include "PersistingPowerupState.h"
#include <map>

//////////////////////////////////////
// Dynamic Checkpoint State Structures
//////////////////////////////////////

struct CharacterCheckpointState
{
	float m_lyrielHealth;
    float m_deathHealth;
    float m_reaperGaugeAmount;
    bool m_unlockedPowerups[static_cast<int>(PowerupId::Count)];
};

struct EnemyCheckpointState
{
    //UID m_enemyId;
    bool m_isDead;
};

struct BreakableCheckpointState
{
	//UID m_breakableId;
    bool m_isBroken;
};

struct CollectibleCheckpointState
{
	//UID m_pickupId;
    bool m_isCollected;
};

/////////////////////////////
// Hardcoded Checkpoint State
/////////////////////////////

enum CheckpointId // Represents different checkpoints in the game, useful because they are partially harcoded
{
	NONE = 0,
    CHECKPOINT_1_LEVEL_1= 100,
    CHECKPOINT_2_LEVEL_1,
    CHECKPOINT_3_LEVEL_1,

	CHECKPOINT_1_LEVEL_2 = 200,

	CHECKPOINT_1_LEVEL_3 = 300,
	//... Add more checkpoints as needed
};

//struct CheckpointDef 
//{
//    Vector3 spawnPos;
//
//    std::vector<uint64_t> doorsToOpen;
//    //puzzles superados, ver como guardar (id de puzle? y enviarselo al puzzle manager?)
//};

class CheckpointSetup;

class CheckpointManager
{
public:
    static CheckpointManager& Get();

    template <typename T>
    void SaveState(UID id, const T& checkpointState)
    {
        if constexpr (std::is_same_v<T, CharacterCheckpointState>)
        {
            m_characterCheckpoints.insert_or_assign(id, checkpointState);
        }
        else if constexpr (std::is_same_v<T, EnemyCheckpointState>)
        {
            m_enemyCheckpoints.insert({ id, checkpointState });
        }
        else if constexpr (std::is_same_v<T, BreakableCheckpointState>)
        {
            m_breakableCheckpoints.insert({ id, checkpointState });
        }
        else if constexpr (std::is_same_v<T, CollectibleCheckpointState>)
        {
            m_collectibleCheckpoints.insert({ id, checkpointState });
        }
        else
        {
			//assert(false && "Unsupported checkpoint state type");
        }
    }

	void SetCheckpointId(CheckpointId checkpointId) { m_lastCheckpointId = checkpointId; }

    void RegisterCheckpoint(const CheckpointId& checkpointId, CheckpointSetup* setup)
    {
        m_checkpointSetupPool[checkpointId] = setup;
    }

    void ApplyCheckpoint();

private:
    void LoadCheckpointByID();

private:
	CheckpointId m_lastCheckpointId = CheckpointId::NONE;

	std::map<CheckpointId, CheckpointSetup*> m_checkpointSetupPool;

	std::map<UID, CharacterCheckpointState> m_characterCheckpoints;
    std::map<UID, EnemyCheckpointState> m_enemyCheckpoints;
    std::map<UID, BreakableCheckpointState> m_breakableCheckpoints;
    std::map<UID, CollectibleCheckpointState> m_collectibleCheckpoints;
};
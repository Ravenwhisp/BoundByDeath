#pragma once

#include "ScriptAPI.h"
#include "PersistingPowerupState.h"
#include <map>

//////////////////////////////////////
// Dynamic Checkpoint State Structures
//////////////////////////////////////

struct GlobalCheckpointState
{
    float m_reaperGaugeAmount;
    bool m_unlockedPowerups[static_cast<int>(PowerupId::Count)];
    //more to add?
};

struct CharacterCheckpointState
{
    float m_health;
};

struct EnemyCheckpointState
{
    //UID m_enemyId;
	bool m_isDead; //esto realmente es necesario? si creamos el state cuando muere, no hace falta esto ya que ESTARÁ muerto, osea solo por tener su estado significa que esta muerto
    //posicion donde murieron?
};

struct BreakableCheckpointState
{
	//UID m_breakableId;
    bool m_isBroken; //lo mismo, si esto no es necesario bastará con una lista de uid
};

struct CollectibleCheckpointState
{
	//UID m_pickupId;
    bool m_isCollected; //lo mismo, si esto no es necesario bastará con una lista de uid
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

class CheckpointSetup;

class CheckpointManager
{
public:
    static CheckpointManager& Get();

    void Register(GameObject* go)
    {
        if (go) m_registeredGameObjects[go->GetID()] = go;
    }

    void Unregister(GameObject* go)
    {
        if (go) m_registeredGameObjects.erase(go->GetID());
    }

    GameObject* FindGameObject(UID uid)
    {
        auto it = m_registeredGameObjects.find(uid);
        return (it != m_registeredGameObjects.end()) ? it->second : nullptr;
    }

    template <typename T>
    void SaveState(UID id, const T& checkpointState)
    {
        if constexpr (std::is_same_v<T, GlobalCheckpointState>)
        {
            m_activeGlobalState = checkpointState;
        }
        else if constexpr (std::is_same_v<T, CharacterCheckpointState>)
        {
            m_characterStates.insert_or_assign(id, checkpointState);
        }
        else if constexpr (std::is_same_v<T, EnemyCheckpointState>)
        {
            m_enemyStates.insert({ id, checkpointState });
        }
        else if constexpr (std::is_same_v<T, BreakableCheckpointState>)
        {
            m_breakableStates.insert({ id, checkpointState });
        }
        else if constexpr (std::is_same_v<T, CollectibleCheckpointState>)
        {
            m_collectibleStates.insert({ id, checkpointState });
        }
        else
        {
			//assert(false && "Unsupported checkpoint state type");
        }
    }

    void SetCheckpoint(CheckpointId checkpointId);

    void RegisterCheckpoint(const CheckpointId& checkpointId, CheckpointSetup* setup)
    {
        m_checkpointSetupPool[checkpointId] = setup;
    }

    void ApplyCheckpoint();

private:
    void LoadCheckpointByID();

private:
	CheckpointId m_lastCheckpointId = CheckpointId::NONE;

    std::unordered_map<UID, GameObject*> m_registeredGameObjects;

	std::map<CheckpointId, CheckpointSetup*> m_checkpointSetupPool;

    GlobalCheckpointState m_activeGlobalState;
	std::map<UID, CharacterCheckpointState> m_characterStates;
    std::map<UID, EnemyCheckpointState> m_enemyStates;
    std::map<UID, BreakableCheckpointState> m_breakableStates;
    std::map<UID, CollectibleCheckpointState> m_collectibleStates;

    std::map<UID, EnemyCheckpointState> m_enemyActiveStates;
	std::map<UID, BreakableCheckpointState> m_breakableActiveStates;
	std::map<UID, CollectibleCheckpointState> m_collectibleActiveStates;
};
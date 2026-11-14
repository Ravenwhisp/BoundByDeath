#pragma once

#include "ScriptAPI.h"
#include <unordered_map>

class Level2Cheats : public Script
{
    DECLARE_SCRIPT(Level2Cheats)

public:
    explicit Level2Cheats(GameObject* owner);

    void Start() override;
    void Update() override;


    void AutoWin();
    void AutoLose();
    void Teleport();
    void ToggleInvincibility();
    void SpawnEnemy(int enemyPrefabIndex);
    void RestoreHealth();
    void DownState();
    void toLevel1();
    void killEnemies();
    void toLevel2();
    void toBossLevel();
    void FillReaperGauge();

private:

    int m_playerIndex = 0;
    int m_spawnIndex = 0;

    std::vector<Vector3> spawnPoints = {
    Vector3(21.136f, 1.141f, 9.808f),
    Vector3(23.071f, 1.141f, 32.073f),
    Vector3(41.595f, 1.304f, 40.324f),
    Vector3(64.629f, -25.313, 47.059f),
    Vector3(17.207f, -25.313f, 40.831f)
    };

    bool KeyComboPressed(KeyCode mainKey);
    std::unordered_map<KeyCode, bool> m_previousKeyStates;

    //std::vector<AssetRef<Prefab>> m_enemyPrefabPaths;
};


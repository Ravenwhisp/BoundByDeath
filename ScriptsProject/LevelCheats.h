#pragma once

#include "ScriptAPI.h"
#include <unordered_map>

class LevelCheats : public Script
{
    DECLARE_SCRIPT(LevelCheats)

public:
    explicit LevelCheats(GameObject* owner);

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
    Vector3(11.860f, 1.309f, 30.748f),
    Vector3(29.945f, 1.247f, -5.625f),
    //Vector3(-8.101f, 1.247f, -11.322f),
    Vector3(-10.452f, -1.760f, 28.630f),
    Vector3(-25.191f, 1.284f, -14.736f),
    Vector3(-64.729f, -23.842f, 27.340f),
    Vector3(-44.593f, -17.913f, 55.397f),
    Vector3(-43.667f, -17.711f, 134.018f)
    };

	bool KeyComboPressed(KeyCode mainKey);
    std::unordered_map<KeyCode, bool> m_previousKeyStates;

    //std::vector<AssetRef<Prefab>> m_enemyPrefabPaths;
};


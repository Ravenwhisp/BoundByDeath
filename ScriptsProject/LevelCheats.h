#pragma once

#include "ScriptAPI.h"

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
	void ActivateGodMode();
	void SpawnEnemies();
	void RestoreHealth();
	void DownState();
	void restartLevel();

    //ScriptFieldList getExposedFields() const override;
};


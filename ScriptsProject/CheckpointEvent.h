#pragma once

#include "ScriptAPI.h"

#include "GameplayEventAction.h"

#include "CheckpointManager.h"

class GameplayEventTrigger;
class ReaperGauge;
class Damageable;

class CheckpointEvent : public GameplayEventAction
{
    DECLARE_SCRIPT(CheckpointEvent)

public:
    explicit CheckpointEvent(GameObject* owner);

	void executeEvent(GameplayEventTrigger* trigger) override;

    void Start() override;
    void Update() override;

protected:
	CheckpointManager* m_checkpointManager = nullptr;
    ReaperGauge* m_reaperGauge = nullptr;
    Damageable* m_lyrielDamageable = nullptr;
	Damageable* m_deathDamageable = nullptr;

	CheckpointId m_checkpointId = CheckpointId::NONE;

};
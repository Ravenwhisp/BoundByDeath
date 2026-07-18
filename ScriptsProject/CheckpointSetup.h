#pragma once

#include "ScriptAPI.h"

#include "CheckpointManager.h"

class ReaperGauge;

class CheckpointSetup : public Script
{
    DECLARE_SCRIPT(CheckpointSetup)

public:
    explicit CheckpointSetup(GameObject* owner);

    void Start() override;

    virtual void ApplyCheckpointState() {};

private:
	CheckpointManager*  m_checkpointManager = nullptr;
	CheckpointId        m_checkpointId = CheckpointId::NONE;
};
#pragma once

#include "ScriptAPI.h"

#include "CheckpointManager.h"

class ReaperGauge;

class CheckpointSetup : public Script
{
    DECLARE_SCRIPT(CheckpointSetup)

public:
    explicit CheckpointSetup(GameObject* owner);

    FieldList getExposedFields() const override;

    void Start() override;

    virtual void ApplyCheckpointState();

public:
    ComponentRef<Transform> m_lyrielTransform;
    ComponentRef<Transform> m_deathTransform;

private:
	CheckpointManager*  m_checkpointManager = nullptr;
	CheckpointId        m_checkpointId = CheckpointId::NONE;

    Vector3 lyrielSpawnPosition;
    Vector3 deathSpawnPosition;
};
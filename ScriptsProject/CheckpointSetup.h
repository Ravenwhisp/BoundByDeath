#pragma once

#include "ScriptAPI.h"

#include "PersistingCheckpointState.h"

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
	PersistingCheckpointState*  m_PersistingCheckpointState = nullptr;
	CheckpointId        m_checkpointId = CheckpointId::NONE;

    Vector3 lyrielSpawnPosition;
    Vector3 deathSpawnPosition;
};
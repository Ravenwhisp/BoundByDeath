#pragma once

#include "ScriptAPI.h"
#include "ParticleLifecycle.h"

class TeleportVFXTester final : public Script
{
    DECLARE_SCRIPT(TeleportVFXTester)

public:
    explicit TeleportVFXTester(GameObject* owner);

    void Start() override;
    void Update() override;
    void OnGameStop() override;

    FieldList getExposedFields() const override;

public:
    PrefabRef m_effectPrefab;
    float m_interval = 2.5f;
    float m_effectLifetime = 2.0f;
    float m_jumpDistance = 5.0f;
    bool m_playArrival = true;

private:
    void playTeleport();

    ParticleLifecycle::TimedParticleTracker m_tracker;
    Vector3 m_origin = Vector3::Zero;
    float m_timer = 0.0f;
    bool m_onOtherSide = false;
};

#pragma once

#include "ScriptAPI.h"
#include "HapticEffectDefinition.h"

class Transform;
class Damageable;
class PlayerController;

enum class HeartbeatPhase : uint8_t
{
    Idle,
    Lub,
    InterBeat,
    Dub,
    Diastole,
};

struct PlayerHeartbeatState
{
    HeartbeatPhase phase = HeartbeatPhase::Idle;
    float          timer = 0.0f;
    unsigned int   seed = 0;
};

class Bound : public Script
{
    DECLARE_SCRIPT(Bound)

public:
    explicit Bound(GameObject* owner);

    void Start() override;
    void Update() override;

    void drawGizmo() override;

    ScriptFieldList getExposedFields() const override;

public:
    ScriptComponentRef<Transform> m_firstTarget;
    ScriptComponentRef<Transform> m_secondTarget;

    ScriptComponentRef<Transform> m_BoundUI;

    Damageable* m_firstDamageable = nullptr;
    Damageable* m_secondDamageable = nullptr;

    float m_minDistance = 70.0f;
    float m_distanceDamage = 80.0f;
    float m_distanceInstaKill = 100.0f;

    float baseDamage = 20.0f;
    float maxDamage = 40.0f;

    float m_radiusThreshold = 2.0f;

    Vector3 m_center = Vector3(0.0f, 0.0f, 0.0f);
    float   m_currentRadius = 0.0f;

    float m_previousDistance = 0.0f;

private:
    PlayerController* m_firstController = nullptr;
    PlayerController* m_secondController = nullptr;

    PlayerHeartbeatState m_hb1;
    PlayerHeartbeatState m_hb2;

    void tickHeartbeat(PlayerHeartbeatState& hb, float normDist, int playerIndex, float dt);

    void fireLub(float intensity, int playerIndex);
    void fireDub(float intensity, int playerIndex);
};


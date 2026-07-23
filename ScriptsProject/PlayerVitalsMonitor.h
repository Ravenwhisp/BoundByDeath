#pragma once

#include "ScriptAPI.h"

class PlayerDamageable;
class Bound;

class PlayerVitalsMonitor : public Script
{
    DECLARE_SCRIPT(PlayerVitalsMonitor)

public:
    explicit PlayerVitalsMonitor(GameObject* owner);

    void Start() override;
    void Update() override;

    FieldList getExposedFields() const override;

public:
    bool  m_autoEnableHeartbeat = true;
    float m_healthThreshold = 0.5f;
    float m_deathGreyDuration = 1.5f;
    float m_deathBlackDuration = 1.5f;

private:
    void findPlayers();
    void findBound();

    bool allPlayersFound() const;

private:
    PlayerDamageable* m_player1 = nullptr;
    PlayerDamageable* m_player2 = nullptr;

    Bound* m_bound = nullptr;
};

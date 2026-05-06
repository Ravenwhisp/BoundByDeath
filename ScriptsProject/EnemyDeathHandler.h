#pragma once

#include "ScriptAPI.h"

class Damageable;

class EnemyDeathHandler : public Script
{
    DECLARE_SCRIPT(EnemyDeathHandler)

public:
    explicit EnemyDeathHandler(GameObject* owner);

    void Start() override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

private:
    void playDeathAnimation();
    void startDestroyCountdown(float delay);
    void destroyEnemyNow();
    void processDeath();
    void DropHealth();

public:
    float m_destroyDelay = 2.0f;
    std::string m_deathStateName = "";
    std::string m_healthPrefabPath = "";
    int m_healthDropQuantity = 1;
    float m_healthDropAmount = 10.0f;
    float m_dropRadius   = 2.0f;

private:
    Damageable* m_damageable = nullptr;
    bool m_hasProcessedDeath = false;
    bool m_waitingToDestroy = false;
    float m_deathTimer = 0.0f;
};
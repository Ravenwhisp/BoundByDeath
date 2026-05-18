#pragma once

#include "ScriptAPI.h"

class Damageable;
class EnemyDeathHandler;

class LinkedDeathHandler : public Script
{
    DECLARE_SCRIPT(LinkedDeathHandler)

public:
    explicit LinkedDeathHandler(GameObject* owner);

    void Start() override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

    void onLinkedPartnerDied();

public:
    ScriptComponentRef<Transform> m_linkedPartner;
    float m_reviveDelay = 3.0f;
    float m_reviveHpPercent = 0.5f;

private:
    Damageable* m_damageable = nullptr;
    EnemyDeathHandler* m_deathHandler = nullptr;
    LinkedDeathHandler* m_partnerHandler = nullptr;

    float m_deathTimer = 0.0f;
    bool m_isWaitingForPartner = false;
    bool m_hasProcessedDeath = false;
};

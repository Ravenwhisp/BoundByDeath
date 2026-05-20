#pragma once

#include "EnemyDeathHandler.h"

class LinkedDeathEnemyHandler : public EnemyDeathHandler
{
    DECLARE_SCRIPT(LinkedDeathEnemyHandler)

public:
    explicit LinkedDeathEnemyHandler(GameObject* owner);

    void Start() override;
    ScriptFieldList getExposedFields() const override;

public:
    ScriptComponentRef<Transform> m_linkedPartner;
    float m_graceWindow = 3.0f;
    float m_reviveHpPercent = 0.5f;

    void notifyLinkedDeath();
    void notifyLinkedRevive();

private:
    bool canStartDeathProcessing() const override;
    void onDeathProcessingBlocked(float dt) override;
    void onDeathProcessed() override;
    void onDeathStateReset() override;

    bool isPartnerDead() const;
    bool isPartnerWaitingForMe() const;
    void clearPendingState();
    void resolveMutualDeath();

private:
    LinkedDeathEnemyHandler* m_partnerHandler = nullptr;
    bool m_isWaitingForPartner = false;
    float m_pendingDeathTimer = 0.0f;
    bool m_partnerDiedNotification = false;
};

#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class RangedEnemyController;
class ArcherAttackConfig;

class EnemyAttackState : public StateMachineScript
{
    DECLARE_SCRIPT(EnemyAttackState)

public:
    explicit EnemyAttackState(GameObject* owner);

    void OnStateEnter() override;
    void OnStateUpdate() override;
    void OnStateExit() override;

private:
    void resetAttackCycle();
    void performAttack();
    void tryDamageTarget(Transform* targetTransform);

private:
    RangedEnemyController* m_archerController = nullptr;
    ArcherAttackConfig* m_attackConfig = nullptr;

    float m_stateTimer = 0.0f;
    float m_cooldownTimer = 0.0f;

    bool m_hasAppliedDamage = false;
};
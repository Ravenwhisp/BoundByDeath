#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"
#include "RangedEnemyController.h"

class EnemyAttackState : public StateMachineScript
{
    DECLARE_SCRIPT(EnemyAttackState)

public:
    explicit EnemyAttackState(GameObject* owner);

    void OnStateEnter() override;
    void OnStateUpdate() override;
    void OnStateExit() override;

private:
    RangedEnemyController* m_archerController = nullptr;
};
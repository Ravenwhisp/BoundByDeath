#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"
#include "RangedEnemyController.h"

class EnemyIdleState : public StateMachineScript
{
    DECLARE_SCRIPT(EnemyIdleState)

public:
    explicit EnemyIdleState(GameObject* owner);

    void OnStateEnter() override;
    void OnStateUpdate() override;
    void OnStateExit() override;

    ScriptFieldList getExposedFields() const override;

private:
    RangedEnemyController* getRangedEnemyController() const;

public:
    bool m_debugEnabled = true;
};
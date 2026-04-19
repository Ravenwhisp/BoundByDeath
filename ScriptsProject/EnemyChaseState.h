#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"
#include "RangedEnemyController.h"

class EnemyChaseState : public StateMachineScript
{
    DECLARE_SCRIPT(EnemyChaseState)

public:
    explicit EnemyChaseState(GameObject* owner);

    void OnStateEnter() override;
    void OnStateUpdate() override;
    void OnStateExit() override;

    ScriptFieldList getExposedFields() const override;

private:
    RangedEnemyController* getRangedEnemyController() const;

public:
    bool m_debugEnabled = true;
};
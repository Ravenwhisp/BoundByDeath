#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class EnemyState : public StateMachineScript
{
    DECLARE_SCRIPT(EnemyState)

public:
    explicit EnemyState(GameObject* owner);

    void OnStateEnter() override;
    void OnStateUpdate() override;
    void OnStateExit() override;

   /* ScriptFieldList getExposedFields() const override;*/
};
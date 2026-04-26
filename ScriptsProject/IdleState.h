#pragma once

#include "ScriptAPI.h"
#include "EnemyState.h"

class IdleState : public EnemyState
{
    DECLARE_SCRIPT(IdleState)

public:
    explicit IdleState(GameObject* owner);

    void OnStateEnter() override;
    void OnStateUpdate() override;
    void OnStateExit() override;

    ScriptFieldList getExposedFields() const override;

    bool m_debugEnabled = true;
};
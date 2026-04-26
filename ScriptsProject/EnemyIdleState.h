#pragma once

#include "ScriptAPI.h"
#include "EnemyState.h"

class EnemyIdleState : public EnemyState
{
    DECLARE_SCRIPT(IdleState)

public:
    explicit EnemyIdleState(GameObject* owner);

    void OnStateEnter() override;
    void OnStateUpdate() override;
    void OnStateExit() override;

    ScriptFieldList getExposedFields() const override;

    bool m_debugEnabled = true;
};
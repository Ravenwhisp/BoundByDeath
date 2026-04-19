#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class EnemyDeathState : public StateMachineScript
{
    DECLARE_SCRIPT(EnemyDeathState)

public:
    explicit EnemyDeathState(GameObject* owner);

    void OnStateEnter() override;
    void OnStateUpdate() override;
    void OnStateExit() override;

    ScriptFieldList getExposedFields() const override;

private:
    float m_elapsedTime = 0.0f;
    bool m_destroyRequested = false;

public:
    float m_fallbackDestroyDelay = 2.5f;
    bool m_useAnimationDuration = true;
    float m_extraDestroyDelay = 0.1f;
    bool m_debugEnabled = true;
};
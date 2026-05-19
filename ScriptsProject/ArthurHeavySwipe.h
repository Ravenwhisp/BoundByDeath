#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class ArthurBossController;
class ArthurAttackConfig;
class ArthurAttackExecutor;

class ArthurHeavySwipe : public StateMachineScript
{
    DECLARE_SCRIPT(ArthurHeavySwipe)

public:
    explicit ArthurHeavySwipe(GameObject* owner);

    void OnStateEnter() override;
    void OnStateUpdate() override;
    void OnStateExit() override;

private:
    void tryApplyHit(int hitIndex);
    void goToRecover();

private:
    ArthurBossController* m_arthurController = nullptr;
    ArthurAttackConfig* m_attackConfig = nullptr;
    ArthurAttackExecutor* m_attackExecutor = nullptr;

    float m_stateTimer = 0.0f;

    bool m_hit1Applied = false;
    bool m_hit2Applied = false;
    bool m_hit3Applied = false;
};
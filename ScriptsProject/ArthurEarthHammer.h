#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class ArthurBossController;
class ArthurAttackConfig;

class ArthurEarthHammer : public StateMachineScript
{
    DECLARE_SCRIPT(ArthurEarthHammer)

public:
    explicit ArthurEarthHammer(GameObject* owner);

    void OnStateEnter() override;
    void OnStateUpdate() override;
    void OnStateExit() override;

private:
    void applyImpact();
    void tryDamageTarget(Transform* targetTransform);
    bool isTargetInsideRadius(Transform* targetTransform) const;
    void goToRecover();

private:
    ArthurBossController* m_arthurController = nullptr;
    ArthurAttackConfig* m_attackConfig = nullptr;

    float m_stateTimer = 0.0f;

    bool m_hasAppliedImpact = false;
};
#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class ArthurBossController;

class ArthurHeavySwipe : public StateMachineScript
{
    DECLARE_SCRIPT(ArthurHeavySwipe)

public:
    explicit ArthurHeavySwipe(GameObject* owner);

    void OnStateEnter() override;
    void OnStateUpdate() override;
    void OnStateExit() override;

    ScriptFieldList getExposedFields() const override;

private:
    void tryApplyHit(int hitIndex);
    void tryDamageTarget(Transform* targetTransform, int hitIndex);
    bool isTargetInsideHitCone(Transform* targetTransform) const;
    void goToRecover();

public:
    float m_damage = 10.0f;

    float m_attackRange = 3.0f;
    float m_attackHalfAngleDegrees = 60.0f;

    float m_totalDuration = 1.5f;

    float m_hit1Time = 0.45f;
    float m_hit2Time = 0.95f;
    float m_hit3Time = 1.35f;

    float m_recoveryDuration = 0.75f;

    bool m_debugEnabled = true;

private:
    ArthurBossController* m_arthurController = nullptr;

    float m_stateTimer = 0.0f;

    bool m_hit1Applied = false;
    bool m_hit2Applied = false;
    bool m_hit3Applied = false;
};
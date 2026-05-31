#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class RangedEnemyController;
class ArcherAttackConfig;
class AnimationComponent;

class ArcherAttackState : public StateMachineScript
{
    DECLARE_SCRIPT(ArcherAttackState)

public:
    explicit ArcherAttackState(GameObject* owner);

    void OnStateEnter() override;
    void OnStateUpdate() override;
    void OnStateExit() override;

private:
    void tryDamageTarget(Transform* targetTransform);

private:
    RangedEnemyController* m_archerController = nullptr;
    ArcherAttackConfig* m_attackConfig = nullptr;
    AnimationComponent* m_animation = nullptr;

    Transform* m_committedTarget = nullptr;

    float m_stateTimer = 0.0f;
    bool m_hasAppliedDamage = false;
};
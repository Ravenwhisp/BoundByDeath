#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class EnemyController;
class AnimationComponent;

class EnemyState : public StateMachineScript
{
    DECLARE_SCRIPT(EnemyState)

public:
    explicit EnemyState(GameObject* owner);

    void OnStateEnter() override;
    void OnStateUpdate() override;
    void OnStateExit() override;

protected:
    EnemyController* GetEnemyController() const;
    AnimationComponent* GetAnimationComponent() const;

    bool TryHandleDeath();
    bool SendTrigger(const char* triggerName);
};
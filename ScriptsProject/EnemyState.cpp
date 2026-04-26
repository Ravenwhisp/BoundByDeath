#include "pch.h"
#include "EnemyState.h"

#include "EnemyController.h"

EnemyState::EnemyState(GameObject* owner)
    : StateMachineScript(owner)
{
}

void EnemyState::OnStateEnter()
{
}

void EnemyState::OnStateUpdate()
{
}

void EnemyState::OnStateExit()
{
}


AnimationComponent* EnemyState::GetAnimationComponent() const
{
    return AnimationAPI::getAnimationComponent(getOwner());
}

bool EnemyState::TryHandleDeath()
{
    EnemyController* controller = GetEnemyController();

    if (!controller || !controller->IsDead())
    {
        return false;
    }

    return SendTrigger("Die");
}

bool EnemyState::SendTrigger(const char* triggerName)
{
    AnimationComponent* animation = GetAnimationComponent();

    if (!animation)
    {
        return false;
    }

    return AnimationAPI::sendTrigger(animation, triggerName);
}

IMPLEMENT_SCRIPT(EnemyState)
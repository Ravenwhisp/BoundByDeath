#include "pch.h"
#include "EnemyIdleState.h"

#include "RangedEnemyController.h"

EnemyIdleState::EnemyIdleState(GameObject* owner)
    : StateMachineScript(owner)
{
}

void EnemyIdleState::OnStateEnter()
{
    m_archerController = GameObjectAPI::findScript<RangedEnemyController>(getOwner());

    if (!m_archerController)
    {
        Debug::error("[EnemyIdle] RangedEnemyController not found.");
        return;
    }

    Debug::log("[EnemyIdleState] ENTER");
}

void EnemyIdleState::OnStateUpdate()
{
    if (!m_archerController)
    {
        return;
    }

    if (!m_archerController->hasTarget())
    {
        return;
    }

    AnimationComponent* animation = AnimationAPI::getAnimationComponent(getOwner());
    if (!animation)
    {
        return;
    }

    AnimationAPI::sendTrigger(animation, "ToChase");

    Debug::log("[EnemyIdleState] Chase trigger sent");
}

void EnemyIdleState::OnStateExit()
{
    Debug::log("[EnemyIdleState] EXIT");
}

IMPLEMENT_SCRIPT(EnemyIdleState)
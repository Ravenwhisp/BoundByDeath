#include "pch.h"
#include "EnemyAttackState.h"

#include "RangedEnemyController.h"

EnemyAttackState::EnemyAttackState(GameObject* owner)
    : StateMachineScript(owner)
{
}

void EnemyAttackState::OnStateEnter()
{
    m_archerController = GameObjectAPI::findScript<RangedEnemyController>(getOwner());

    if (!m_archerController)
    {
        Debug::error("[EnemyAttackState] RangedEnemyController not found.");
        return;
    }

    Debug::log("[EnemyAttackState] ENTER");
}

void EnemyAttackState::OnStateUpdate()
{
    if (!m_archerController)
    {
        return;
    }

    AnimationComponent* animation = AnimationAPI::getAnimationComponent(getOwner());
    if (!animation)
    {
        return;
    }

    if (!m_archerController->hasTarget())
    {
        AnimationAPI::sendTrigger(animation, "Idle");

        Debug::log("[EnemyAttackState] Idle trigger sent");

        return;
    }

    if (!m_archerController->isTargetInAttackRange())
    {
        AnimationAPI::sendTrigger(animation, "Chase");

        Debug::log("[EnemyAttackState] Chase trigger sent");

        return;
    }

    // Basic attack logic will go here next.
}

void EnemyAttackState::OnStateExit()
{
    Debug::log("[EnemyAttackState] EXIT");
}

IMPLEMENT_SCRIPT(EnemyAttackState)
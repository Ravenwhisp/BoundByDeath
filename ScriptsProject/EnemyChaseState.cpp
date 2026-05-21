#include "pch.h"
#include "EnemyChaseState.h"

#include "RangedEnemyController.h"

EnemyChaseState::EnemyChaseState(GameObject* owner)
    : StateMachineScript(owner)
{
}

void EnemyChaseState::OnStateEnter()
{
    m_archerController = GameObjectAPI::findScript<RangedEnemyController>(getOwner());

    if (!m_archerController)
    {
        Debug::error("[EnemyChaseState] RangedEnemyController not found.");
        return;
    }

    Debug::log("[EnemyChaseState] ENTER");
}

void EnemyChaseState::OnStateUpdate()
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

        Debug::log("[EnemyChaseState] Idle trigger sent");

        return;
    }

    if (m_archerController->isTargetInAttackRange())
    {
        AnimationAPI::sendTrigger(animation, "Attack");

        Debug::log("[EnemyChaseState] Attack trigger sent");

        return;
    }

    m_archerController->moveTowardsTarget();
}

void EnemyChaseState::OnStateExit()
{
    Debug::log("[EnemyChaseState] EXIT");

    if (!m_archerController)
    {
        return;
    }

    m_archerController->clearPath();
}

IMPLEMENT_SCRIPT(EnemyChaseState)
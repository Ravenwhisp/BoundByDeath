#include "pch.h"
#include "EnemyIdleState.h"

IMPLEMENT_SCRIPT_FIELDS(EnemyIdleState,
    SERIALIZED_BOOL(m_debugEnabled, "Debug Enabled")
)

EnemyIdleState::EnemyIdleState(GameObject* owner)
    : StateMachineScript(owner)
{
}

void EnemyIdleState::OnStateEnter()
{
    if (!m_debugEnabled)
    {
        return;
    }

    Debug::log("[EnemyIdleState] ENTER");
}

void EnemyIdleState::OnStateUpdate()
{
    RangedEnemyController* controller = getRangedEnemyController();
    if (!controller)
    {
        return;
    }

    if (controller->TrySendDeathTrigger())
    {
        return;
    }

    if (!controller->IsTargetDetected())
    {
        return;
    }


    AnimationComponent* animation = AnimationAPI::getAnimationComponent(getOwner());
    if (!animation)
    {
        return;
    }

    AnimationAPI::sendTrigger(animation, "Chase");

    if (m_debugEnabled)
    {
        Debug::log("[EnemyIdleState] Chase trigger sent");
    }
}

void EnemyIdleState::OnStateExit()
{
    if (!m_debugEnabled)
    {
        return;
    }

    Debug::log("[EnemyIdleState] EXIT");
}

RangedEnemyController* EnemyIdleState::getRangedEnemyController() const
{
    return GameObjectAPI::findScript<RangedEnemyController>(getOwner());
}

IMPLEMENT_SCRIPT(EnemyIdleState)
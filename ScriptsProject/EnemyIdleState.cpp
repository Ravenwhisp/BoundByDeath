#include "pch.h"
#include "EnemyIdleState.h"

static const ScriptFieldInfo IDLEFields[] =
{
    { "Debug Enabled", ScriptFieldType::Bool, offsetof(EnemyIdleState, m_debugEnabled) }
};

IMPLEMENT_SCRIPT_FIELDS(EnemyIdleState, IDLEFields)

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
    Script* script = GameObjectAPI::getScript(getOwner(), "RangedEnemyController");
    return dynamic_cast<RangedEnemyController*>(script);
}

IMPLEMENT_SCRIPT(EnemyIdleState)
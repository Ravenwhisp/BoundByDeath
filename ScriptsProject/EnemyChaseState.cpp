#include "pch.h"
#include "EnemyChaseState.h"

static const ScriptFieldInfo CHASEFields[] =
{
    { "Debug Enabled", ScriptFieldType::Bool, offsetof(EnemyChaseState, m_debugEnabled) }
};

IMPLEMENT_SCRIPT_FIELDS(EnemyChaseState, CHASEFields)

EnemyChaseState::EnemyChaseState(GameObject* owner)
    : StateMachineScript(owner)
{
}

void EnemyChaseState::OnStateEnter()
{
    if (!m_debugEnabled)
    {
        return;
    }

    Debug::log("[EnemyChaseState] ENTER");
}

void EnemyChaseState::OnStateUpdate()
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

    AnimationComponent* animation = AnimationAPI::getAnimationComponent(getOwner());
    if (!animation)
    {
        return;
    }

    if (!controller->HasTarget() || controller->IsTargetLost())
    {
        AnimationAPI::sendTrigger(animation, "Idle");

        if (m_debugEnabled)
        {
            Debug::log("[EnemyChaseState] Idle trigger sent");
        }

        return;
    }

    if (controller->IsTargetInAttackRange())
    {
        AnimationAPI::sendTrigger(animation, "Attack");

        if (m_debugEnabled)
        {
            Debug::log("[EnemyChaseState] Attack trigger sent");
        }

        return;
    }

    controller->MoveTowardsTarget();
}

void EnemyChaseState::OnStateExit()
{
    if (!m_debugEnabled)
    {
        return;
    }

    Debug::log("[EnemyChaseState] EXIT");
}

RangedEnemyController* EnemyChaseState::getRangedEnemyController() const
{
    Script* script = GameObjectAPI::getScript(getOwner(), "RangedEnemyController");
    return dynamic_cast<RangedEnemyController*>(script);
}

IMPLEMENT_SCRIPT(EnemyChaseState)
#include "pch.h"
#include "EnemyAttackState.h"

static const ScriptFieldInfo ATTACKFields[] =
{
    { "Debug Enabled", ScriptFieldType::Bool, offsetof(EnemyAttackState, m_debugEnabled) }
};

IMPLEMENT_SCRIPT_FIELDS(EnemyAttackState, ATTACKFields)

EnemyAttackState::EnemyAttackState(GameObject* owner)
    : StateMachineScript(owner)
{
}

void EnemyAttackState::OnStateEnter()
{
    if (!m_debugEnabled)
    {
        return;
    }

    Debug::log("[EnemyAttackState] ENTER");
}

void EnemyAttackState::OnStateUpdate()
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

    if (!controller->HasTarget())
    {
        AnimationAPI::sendTrigger(animation, "Chase");

        if (m_debugEnabled)
        {
            Debug::log("[EnemyAttackState] Chase trigger sent (no target)");
        }

        return;
    }

    if (!controller->IsTargetInAttackRange())
    {
        AnimationAPI::sendTrigger(animation, "Chase");

        if (m_debugEnabled)
        {
            Debug::log("[EnemyAttackState] Chase trigger sent (target out of range)");
        }
    }
}

void EnemyAttackState::OnStateExit()
{
    if (!m_debugEnabled)
    {
        return;
    }

    Debug::log("[EnemyAttackState] EXIT");
}

RangedEnemyController* EnemyAttackState::getRangedEnemyController() const
{
    Script* script = GameObjectAPI::getScript(getOwner(), "RangedEnemyController");
    return dynamic_cast<RangedEnemyController*>(script);
}

IMPLEMENT_SCRIPT(EnemyAttackState)
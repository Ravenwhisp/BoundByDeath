#include "pch.h"
#include "IdleState.h"


IMPLEMENT_SCRIPT_FIELDS(IdleState, SERIALIZED_BOOL(m_debugEnabled, "Debug Enabled"))

IdleState::IdleState(GameObject* owner)
    : EnemyState(owner)
{
}

void IdleState::OnStateEnter()
{
}

void IdleState::OnStateUpdate()
{
}

void IdleState::OnStateExit()
{
}

IMPLEMENT_SCRIPT(IdleState)
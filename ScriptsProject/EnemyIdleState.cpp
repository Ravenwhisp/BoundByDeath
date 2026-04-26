#include "pch.h"
#include "EnemyIdleState.h"


IMPLEMENT_SCRIPT_FIELDS(EnemyIdleState, SERIALIZED_BOOL(m_debugEnabled, "Debug Enabled"))

EnemyIdleState::EnemyIdleState(GameObject* owner)
    : EnemyState(owner)
{
}

void EnemyIdleState::OnStateEnter()
{
}

void EnemyIdleState::OnStateUpdate()
{
}

void EnemyIdleState::OnStateExit()
{
}

IMPLEMENT_SCRIPT(EnemyIdleState)
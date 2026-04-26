#include "pch.h"
#include "EnemyState.h"

//static const ScriptFieldInfo EnemyStateFields[] =
//{
//};
//
//IMPLEMENT_SCRIPT_FIELDS(EnemyState, EnemyStateFields)

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

IMPLEMENT_SCRIPT(EnemyState)
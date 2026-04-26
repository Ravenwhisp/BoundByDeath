#include "pch.h"
#include "RangedEnemyController.h"


IMPLEMENT_SCRIPT_FIELDS(RangedEnemyController, SERIALIZED_BOOL(m_debugEnabled, "Debug Enabled"))

RangedEnemyController::RangedEnemyController(GameObject* owner)
    : EnemyController(owner)
{
}

void RangedEnemyController::OnStart()
{
}

void RangedEnemyController::OnUpdate()
{
}

IMPLEMENT_SCRIPT(RangedEnemyController)
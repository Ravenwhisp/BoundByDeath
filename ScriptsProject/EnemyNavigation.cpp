#include "pch.h"
#include "EnemyNavigation.h"

static const ScriptFieldInfo EnemyNavigationFields[] =
{
};

IMPLEMENT_SCRIPT_FIELDS(EnemyNavigation, EnemyNavigationFields)

EnemyNavigation::EnemyNavigation(GameObject* owner)
    : Script(owner)
{
}

void EnemyNavigation::Start()
{
}

void EnemyNavigation::Update()
{
}

IMPLEMENT_SCRIPT(EnemyNavigation)
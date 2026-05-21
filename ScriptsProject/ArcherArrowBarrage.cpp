#include "pch.h"
#include "ArcherArrowBarrage.h"

static const ScriptFieldInfo ArcherArrowBarrageFields[] =
{
};

IMPLEMENT_SCRIPT_FIELDS(ArcherArrowBarrage, ArcherArrowBarrageFields)

ArcherArrowBarrage::ArcherArrowBarrage(GameObject* owner)
    : Script(owner)
{
}

void ArcherArrowBarrage::Start()
{
}

void ArcherArrowBarrage::Update()
{
}

IMPLEMENT_SCRIPT(ArcherArrowBarrage)
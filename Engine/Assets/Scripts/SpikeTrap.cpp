#include "pch.h"
#include "SpikeTrap.h"

static const ScriptFieldInfo myScriptFields[] =
{
    { "Speed", ScriptFieldType::Float, offsetof(SpikeTrap, m_speed), { 0.0f, 20.0f, 0.1f } },
    { "Activation Speed" , ScriptFieldType::Float, offsetof(SpikeTrap, a_speed), {0.0f, 50.0f, 0.1f } },
    { "Duration" , ScriptFieldType::Float, offsetof(SpikeTrap, duration), {0.0f, 50.0f, 0.1f } }
};

IMPLEMENT_SCRIPT_FIELDS(SpikeTrap, myScriptFields)

SpikeTrap::SpikeTrap(GameObject* owner)
    : Script(owner)
{
}

void SpikeTrap::Start()
{
}

void SpikeTrap::Update()
{
}

void SpikeTrap::TrapLoop()
{

}

IMPLEMENT_SCRIPT(SpikeTrap)
#include "pch.h"
#include "LyrielCharacter.h"
#include "ArrowPool.h"

static const ScriptFieldInfo LyrielCharacterFields[] =
{
    { "Player Index", ScriptFieldType::Int, offsetof(LyrielCharacter, m_playerIndex) },
    { "Arrow Spawn Child Name", ScriptFieldType::String, offsetof(LyrielCharacter, m_arrowSpawnChildName) }
};

IMPLEMENT_SCRIPT_FIELDS(LyrielCharacter, LyrielCharacterFields)

LyrielCharacter::LyrielCharacter(GameObject* owner)
    : CharacterBase(owner)
{
}

void LyrielCharacter::Start()
{
    CharacterBase::Start();

    Script* arrowPoolScript = GameObjectAPI::getScript(getOwner(), "ArrowPool");
    m_arrowPool = dynamic_cast<ArrowPool*>(arrowPoolScript);

    if (m_arrowPool == nullptr)
    {
        Debug::log("[LyrielCharacter] ArrowPool not found on owner '%s'.", GameObjectAPI::getName(getOwner()));
    }
}

IMPLEMENT_SCRIPT(LyrielCharacter)
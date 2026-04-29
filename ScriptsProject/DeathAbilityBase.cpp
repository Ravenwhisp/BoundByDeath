#include "pch.h"
#include "DeathAbilityBase.h"
#include "DeathCharacter.h"

DeathAbilityBase::DeathAbilityBase(GameObject* owner)
    : AbilityBase(owner)
{
}

void DeathAbilityBase::Start()
{
    AbilityBase::Start();

    m_deathChar = static_cast<DeathCharacter*>(GameObjectAPI::getScript(getOwner(), "DeathCharacter"));
    m_character = m_deathChar;

    if (m_deathChar == nullptr)
    {
        Debug::log("[DeathAbilityBase] DeathCharacter not found on owner '%s'.", GameObjectAPI::getName(getOwner()));
    }
}
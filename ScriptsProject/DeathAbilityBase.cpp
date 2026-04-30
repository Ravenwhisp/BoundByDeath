#include "pch.h"
#include "DeathAbilityBase.h"
#include "DeathCharacter.h"

DeathAbilityBase::DeathAbilityBase(GameObject* owner)
    : AbilityBase(owner)
{
}

void DeathAbilityBase::Start()
{
    m_deathCharacter = static_cast<DeathCharacter*>(GameObjectAPI::getScript(getOwner(), "DeathCharacter"));
    m_character = m_deathCharacter;

    if (m_deathCharacter == nullptr)
    {
        Debug::log("[DeathAbilityBase] DeathCharacter not found on owner '%s'.", GameObjectAPI::getName(getOwner()));
    }
}
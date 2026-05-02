#include "pch.h"
#include "DeathAbilityBase.h"

#include "DeathCharacter.h"
#include "PlayerState.h"

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

void DeathAbilityBase::releaseComboMoveLock()
{
    m_movementLockedForCombo = false;

    // Another ability may still be holding the lock (e.g. basic attack window still active).
    // Leave PlayerState alone — that ability's finishAttackWindow will release it.
    if (m_character != nullptr && m_character->isUsingAbility())
        return;

    PlayerState* ps = m_character ? m_character->getPlayerState() : nullptr;
    if (ps != nullptr && ps->isAttacking())
        ps->setState(PlayerStateType::Normal);
}
#include "pch.h"
#include "CharacterBase.h"

#include "PlayerState.h"
#include "PlayerRotation.h"
#include "PlayerAnimationController.h"
#include "PlayerTargetController.h"
#include "Damageable.h"

CharacterBase::CharacterBase(GameObject* owner)
    : Script(owner)
{
}

void CharacterBase::Start()
{
    Script* stateScript = GameObjectAPI::getScript(getOwner(), "PlayerState");
    m_playerState = dynamic_cast<PlayerState*>(stateScript);

    Script* rotationScript = GameObjectAPI::getScript(getOwner(), "PlayerRotation");
    m_playerRotation = dynamic_cast<PlayerRotation*>(rotationScript);

    Script* animationScript = GameObjectAPI::getScript(getOwner(), "PlayerAnimationController");
    m_playerAnimationController = dynamic_cast<PlayerAnimationController*>(animationScript);

    Script* targetControllerScript = GameObjectAPI::getScript(getOwner(), "PlayerTargetController");
    m_targetController = dynamic_cast<PlayerTargetController*>(targetControllerScript);

    Script* damageableScript = GameObjectAPI::getScript(getOwner(), "PlayerDamageable");
    m_damageable = dynamic_cast<Damageable*>(damageableScript);

    if (m_playerState == nullptr)
    {
        Debug::log("[CharacterBase] PlayerState not found on owner '%s'.", GameObjectAPI::getName(getOwner()));
    }

    if (m_playerRotation == nullptr)
    {
        Debug::log("[CharacterBase] PlayerRotation not found on owner '%s'.", GameObjectAPI::getName(getOwner()));
    }

    if (m_playerAnimationController == nullptr)
    {
        Debug::log("[CharacterBase] PlayerAnimationController not found on owner '%s'.", GameObjectAPI::getName(getOwner()));
    }

    if (m_targetController == nullptr)
    {
        Debug::log("[CharacterBase] PlayerTargetController not found on owner '%s'.", GameObjectAPI::getName(getOwner()));
    }

    if (m_damageable == nullptr)
    {
        Debug::log("[CharacterBase] Damageable not found on owner '%s'.", GameObjectAPI::getName(getOwner()));
    }
}

bool CharacterBase::isDowned() const
{
    return m_playerState != nullptr && m_playerState->isDowned();
}

bool CharacterBase::isUsingAbility() const
{
    return m_playerState != nullptr && m_playerState->isUsingAbility();
}

void CharacterBase::setUsingAbility(bool value)
{
    if (m_playerState != nullptr)
    {
        m_playerState->setUsingAbility(value);
    }
}
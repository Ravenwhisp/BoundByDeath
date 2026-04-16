#include "pch.h"
#include "CharacterBase.h"
#include "PlayerTargetController.h"

CharacterBase::CharacterBase(GameObject* owner)
    : PlayerDamageable(owner)
{
}

void CharacterBase::Start()
{
    PlayerDamageable::Start();

    m_targetController = static_cast<PlayerTargetController*>(
        GameObjectAPI::getScript(m_owner, "PlayerTargetController"));

    m_playerController = GameObjectAPI::getScript(m_owner, "PlayerController");

    if (m_targetController == nullptr)
    {
        Debug::warn("CharacterBase (%s): PlayerTargetController not found.", GameObjectAPI::getName(m_owner));
    }

    if (m_playerController == nullptr)
    {
        Debug::warn("CharacterBase (%s): PlayerController not found.", GameObjectAPI::getName(m_owner));
    }
}

void CharacterBase::Update()
{
    PlayerDamageable::Update();
}

void CharacterBase::onDeath()
{
    PlayerDamageable::onDeath();
    m_canAct = false;
}

void CharacterBase::onRevive()
{
    PlayerDamageable::onRevive();
    m_canAct = true;
}

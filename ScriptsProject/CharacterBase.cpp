#include "pch.h"
#include "CharacterBase.h"
#include "PlayerTargetController.h"

// No IMPLEMENT_SCRIPT_FIELDS / IMPLEMENT_SCRIPT — abstract class, never registered in the engine.

CharacterBase::CharacterBase(GameObject* owner)
    : Damageable(owner)
{
}

void CharacterBase::Start()
{
    // Initialise HP through Damageable (sets m_currentHp = m_maxHp, etc.)
    Damageable::Start();

    // Obtain sibling scripts from the same GameObject.
    m_targetController = static_cast<PlayerTargetController*>(
        GameObjectAPI::getScript(m_owner, "PlayerTargetController"));

    // PlayerController is an engine script — stored as Script* and only used
    // through ComponentAPI::setActive to stop/restore movement.
    m_playerController = GameObjectAPI::getScript(m_owner, "PlayerController");

    if (m_targetController == nullptr)
    {
        Debug::warn("CharacterBase (%s): PlayerTargetController not found on this GameObject.",
            GameObjectAPI::getName(m_owner));
    }

    if (m_playerController == nullptr)
    {
        Debug::warn("CharacterBase (%s): PlayerController not found on this GameObject.",
            GameObjectAPI::getName(m_owner));
    }
}

void CharacterBase::Update()
{
    Damageable::Update();
}

void CharacterBase::onDamaged(float amount)
{
    Damageable::onDamaged(amount);
    // TODO: trigger hit-reaction animation, VFX, sound
}

void CharacterBase::onDeath()
{
    Damageable::onDeath();

    // Prevent any new ability from activating while the character is dead.
    // Any ability currently executing will cancel itself via AbilityBase::Update().
    m_canAct = false;

    // TODO: disable PlayerController movement on death.
    // ComponentAPI::setActive requires Component*, but Script* has no implicit conversion.
    // Needs engine API support or a dedicated disableMovement() call.
}

void CharacterBase::onRevive()
{
    Damageable::onRevive();

    // Allow abilities to be used again.
    m_canAct = true;

    // TODO: re-enable PlayerController movement on revive (see onDeath TODO).
}

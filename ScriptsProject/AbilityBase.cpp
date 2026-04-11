#include "pch.h"
#include "AbilityBase.h"
#include "CharacterBase.h"
#include "Damageable.h"
#include "PlayerTargetController.h"

// No IMPLEMENT_SCRIPT_FIELDS / IMPLEMENT_SCRIPT — abstract class, never registered in the engine.

AbilityBase::AbilityBase(GameObject* owner)
    : Script(owner)
{
}

void AbilityBase::Start()
{
    // m_character is assigned by each concrete ability in its own Start(),
    // before or after calling AbilityBase::Start().
}

void AbilityBase::Update()
{
    if (m_character == nullptr)
    {
        return;
    }

    // If the character died while this ability was active, force-cancel it
    // so isActive and canAct are left in a clean state.
    if (m_character->isDead())
    {
        if (m_isActive)
        {
            onDeactivate();
        }
        return;
    }

    // Count the cooldown down to zero each frame.
    if (m_cooldownTimer > 0.0f)
    {
        m_cooldownTimer -= Time::getDeltaTime();

        if (m_cooldownTimer < 0.0f)
        {
            m_cooldownTimer = 0.0f;
        }
    }
}

int AbilityBase::getPlayerIndex() const
{
    return m_character->getPlayerIndex();
}

bool AbilityBase::canActivate() const
{
    if (!m_isEnabled)           return false;
    if (m_isActive)             return false;
    if (m_character == nullptr) return false; //esto no tiene que estar aqui, comprobacion en el start. ya haremos en el refactor
    if (m_character->isDead())  return false;
    if (!m_character->canAct()) return false;
    if (m_cooldownTimer > 0.0f) return false;

    return true;
}

void AbilityBase::onActivate()
{
    m_isActive = true;

    // Block all other abilities on this character until this one finishes.
    m_character->setCanAct(false);
}

void AbilityBase::onDeactivate()
{
    m_isActive = false;

    // Start the cooldown countdown.
    m_cooldownTimer = m_cooldown;

    // Allow other abilities to activate again.
    m_character->setCanAct(true);
}

void AbilityBase::dealDamageToCurrentTarget(float damage) const
{
    if (m_character == nullptr)
    {
        return;
    }

    PlayerTargetController* targetController = m_character->getTargetController();
    if (targetController == nullptr)
    {
        return;
    }

    GameObject* target = targetController->getCurrentTarget();
    if (target == nullptr)
    {
        return;
    }

    Script* script = GameObjectAPI::getScript(target, "Damageable");
    if (script == nullptr)
    {
        return;
    }

    static_cast<Damageable*>(script)->takeDamage(damage);
}

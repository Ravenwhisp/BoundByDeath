#include "pch.h"
#include "BreakableDamageable.h"
#include "BreakableObject.h"

BreakableDamageable::BreakableDamageable(GameObject* owner)
    : Damageable(owner)
{
}

void BreakableDamageable::Start()
{
    Damageable::Start();

    Script* breakableScript = GameObjectAPI::getScript(getOwner(), "BreakableObject");
    m_breakableObject = dynamic_cast<BreakableObject*>(breakableScript);

    if (m_breakableObject == nullptr)
    {
        Debug::warn("[BreakableDamageable] '%s' has no BreakableObject script.", GameObjectAPI::getName(getOwner()));
    }
}

void BreakableDamageable::onDeath()
{
    Damageable::onDeath();

    if (m_breakableObject != nullptr)
    {
        m_breakableObject->breakObject();
    }
}

IMPLEMENT_SCRIPT(BreakableDamageable)
#include "pch.h"
#include "BreakableDamageable.h"
#include "BreakableObject.h"

#include "CheckpointManager.h"

BreakableDamageable::BreakableDamageable(GameObject* owner)
    : Damageable(owner)
{
}

void BreakableDamageable::Start()
{
    Damageable::Start();

	m_breakableObject = GameObjectAPI::findScript<BreakableObject>(getOwner());

    if (m_breakableObject == nullptr)
    {
        Debug::warn("[BreakableDamageable] '%s' has no BreakableObject script.", GameObjectAPI::getName(getOwner()));
    }
}

void BreakableDamageable::onDeath()
{
    Damageable::onDeath();

    CheckpointManager::Get().m_brokenBreakables.push_back(m_owner->GetID());

    if (m_breakableObject != nullptr)
    {
        m_breakableObject->onBreak();
    }
}

IMPLEMENT_SCRIPT(BreakableDamageable)
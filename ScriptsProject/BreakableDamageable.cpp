#include "pch.h"
#include "BreakableDamageable.h"

BreakableDamageable::BreakableDamageable(GameObject* owner)
    : Damageable(owner)
{
}

void BreakableDamageable::onDeath()
{
    Damageable::onDeath();

    Debug::log("[BreakableDamageable] Destroying breakable '%s'.",
        GameObjectAPI::getName(getOwner()));

    GameObjectAPI::removeGameObject(getOwner());
}

IMPLEMENT_SCRIPT(BreakableDamageable)
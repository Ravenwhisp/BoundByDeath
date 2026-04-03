#include "pch.h"
#include "PlayerDamageable.h"

PlayerDamageable::PlayerDamageable(GameObject* owner)
    : Damageable(owner)
{
}

void PlayerDamageable::onHpDepleted()
{
    Debug::log("%s HP depleted. Enter down state.", GameObjectAPI::getName(m_owner));
}

IMPLEMENT_SCRIPT(PlayerDamageable)
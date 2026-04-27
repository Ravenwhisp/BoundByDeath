#include "pch.h"
#include "PowerupCollectible.h"

PowerupCollectible::PowerupCollectible(GameObject* owner)
    : Script(owner)
{
}

void PowerupCollectible::Start()
{
}

void PowerupCollectible::OnTriggerEnter(GameObject* player)
{
    if (m_collected)
    {
        return;
    }

    if (!player || GameObjectAPI::getTag(player) != Tag::PLAYER)
    {
        return;
    }

    m_collected = true;

    Debug::log("PowerUp collected");

    GameObjectAPI::removeGameObject(getOwner());
}

IMPLEMENT_SCRIPT(PowerupCollectible)

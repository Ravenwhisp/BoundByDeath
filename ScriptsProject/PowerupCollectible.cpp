#include "pch.h"
#include "PowerupCollectible.h"

static const char* powerupTargetNames[] =
{
    "Lyriel",
    "Death",
    "Both"
};

IMPLEMENT_SCRIPT_FIELDS(PowerupCollectible,
    SERIALIZED_ENUM_INT(m_targetCharacter, "Target Character", powerupTargetNames, 3)
)

PowerupCollectible::PowerupCollectible(GameObject* owner)
    : Script(owner)
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

    if (!canBeCollectedBy(player))
    {
        return;
    }

    m_collected = true;

    GameObjectAPI::removeGameObject(getOwner());
}

bool PowerupCollectible::canBeCollectedBy(GameObject* player) const
{
    const std::string& name = GameObjectAPI::getName(player);

    switch (m_targetCharacter)
    {
    case LYRIEL:
        return name == "Lyriel";

    case DEATH:
        return name == "Death";

    case BOTH:
        return name == "Lyriel" || name == "Death";

    default:
        return false;
    }
}

IMPLEMENT_SCRIPT(PowerupCollectible)

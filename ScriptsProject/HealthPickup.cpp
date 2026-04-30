#include "pch.h"
#include "HealthPickup.h"
#include "PlayerDamageable.h"

#include <cmath>
static const char* healthTargetNames[] =
{
    "Lyriel",
    "Death",
    "Both"
};

IMPLEMENT_SCRIPT_FIELDS(HealthPickup,
    SERIALIZED_ENUM_INT(m_targetCharacter, "Target Character", healthTargetNames, 3),
    SERIALIZED_FLOAT(m_healAmount, "Heal Amount",   0.0f, 100.0f, 1.0f),
    SERIALIZED_FLOAT(m_pickupRadius, "Pickup Radius", 0.0f,  10.0f, 0.1f),
    SERIALIZED_FLOAT(m_idleSpeed, "Idle Speed", 0.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_horizontalAmplitude, "Horizontal Amplitude", 0.0f, 3.0f, 0.05f),
    SERIALIZED_FLOAT(m_verticalAmplitude, "Vertical Amplitude", 0.0f, 3.0f, 0.05f),
)

HealthPickup::HealthPickup(GameObject* owner)
    : Script(owner)
{
}

void HealthPickup::Start()
{
    m_startPosition = TransformAPI::getGlobalPosition(GameObjectAPI::getTransform(getOwner()));
}

void HealthPickup::Update()
{
    if (m_collected)
    {
       return;
    }
    idleAnimation();

}
void HealthPickup::OnTriggerEnter(GameObject* player){
    Debug::log("HealthPickup triggered by %s", GameObjectAPI::getName(player));
     if (m_collected)
    {
        return;
    }
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
    Debug::log("Player %s entered health pickup trigger", GameObjectAPI::getName(player));
    PlayerDamageable* damageable = static_cast<PlayerDamageable*>(GameObjectAPI::getScript(player, "PlayerDamageable"));

    if (!damageable || damageable->isDead())
    {
        return;
    }

    if (damageable->getCurrentHp() >= damageable->getMaxHp())
    {
        return;
    }
    Debug::log("Player %s can collect health pickup, healing for %f", GameObjectAPI::getName(player), m_healAmount);
    m_collected = true;

    damageable->heal(m_healAmount);

    GameObjectAPI::removeGameObject(getOwner());
}

bool HealthPickup::canBeCollectedBy(GameObject* player) const
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

void HealthPickup::idleAnimation()
{
    m_idleTimer += Time::getDeltaTime();

    const float t = m_idleTimer * m_idleSpeed;

    Vector3 position = m_startPosition;

    position.z += std::sin(t) * m_horizontalAmplitude;
    position.y += std::sin(t * 2.0f) * m_verticalAmplitude;

    TransformAPI::setPosition(GameObjectAPI::getTransform(getOwner()), position);
}


IMPLEMENT_SCRIPT(HealthPickup)

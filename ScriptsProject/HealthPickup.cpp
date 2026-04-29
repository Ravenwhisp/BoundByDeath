#include "pch.h"
#include "HealthPickup.h"
#include "PlayerDamageable.h"

IMPLEMENT_SCRIPT(HealthPickup)

IMPLEMENT_SCRIPT_FIELDS(HealthPickup,
    SERIALIZED_FLOAT(m_healAmount,   "Heal Amount",   0.0f, 100.0f, 1.0f),
    SERIALIZED_FLOAT(m_pickupRadius, "Pickup Radius", 0.0f,  10.0f, 0.1f)
)

HealthPickup::HealthPickup(GameObject* owner)
    : Script(owner)
{
}

void HealthPickup::Start()
{
}

void HealthPickup::Update()
{
    const Transform* myTransform = GameObjectAPI::getTransform(m_owner);
    if (myTransform == nullptr)
    {
        return;
    }

    const Vector3 myPosition = TransformAPI::getGlobalPosition(myTransform);
    const std::vector<GameObject*> players = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER, true);

    for (GameObject* player : players)
    {
        const Transform* playerTransform = GameObjectAPI::getTransform(player);
        if (playerTransform == nullptr)
        {
            continue;
        }

        const Vector3 playerPosition = TransformAPI::getGlobalPosition(playerTransform);
        const float distanceSq = Vector3::DistanceSquared(myPosition, playerPosition);

        if (distanceSq <= m_pickupRadius * m_pickupRadius)
        {
            Script* script = GameObjectAPI::getScript(player, "PlayerDamageable");
            PlayerDamageable* damageable = static_cast<PlayerDamageable*>(script);

            if (damageable != nullptr && !damageable->isDead())
            {
                damageable->heal(m_healAmount);
                GameObjectAPI::removeGameObject(m_owner);
                return;
            }
        }
    }
}

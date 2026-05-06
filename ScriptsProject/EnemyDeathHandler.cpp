#include "pch.h"
#include "EnemyDeathHandler.h"
#include "Damageable.h"
#include "HealthPickup.h"
#include <cmath>

IMPLEMENT_SCRIPT_FIELDS(EnemyDeathHandler,
    SERIALIZED_FLOAT(m_destroyDelay, "Destroy Delay", 0.0f, 30.0f, 0.1f),
    SERIALIZED_STRING(m_deathStateName, "Death State Name"),
    SERIALIZED_STRING(m_healthPrefabPath, "Health Prefab Path"),
    SERIALIZED_INT(m_healthDropQuantity, "Health Drop Quantity"),
    SERIALIZED_FLOAT(m_healthDropAmount, "Health Drop Amount", 0.0f, 100.0f, 1.0f),
    SERIALIZED_FLOAT(m_dropRadius, "Drop Radius", 0.0f, 5.0f, 0.1f)
)

EnemyDeathHandler::EnemyDeathHandler(GameObject* owner)
    : Script(owner)
{
}

void EnemyDeathHandler::Start()
{
    Script* script = GameObjectAPI::getScript(m_owner, "EnemyDamageable");
    m_damageable = dynamic_cast<Damageable*>(script);

    if (m_damageable == nullptr)
    {
        Debug::warn("EnemyDeathHandler on '%s' could not find Damageable.", GameObjectAPI::getName(m_owner));
    }
}

void EnemyDeathHandler::Update()
{
    if (m_damageable == nullptr)
    {
        return;
    }

    if (!m_hasProcessedDeath && m_damageable->isDead())
    {
        DropHealth();
        processDeath();
    }

    if (m_waitingToDestroy)
    {
        m_deathTimer -= Time::getDeltaTime();

        if (m_deathTimer <= 0.0f)
        {
            destroyEnemyNow();
        }
    }
}

void EnemyDeathHandler::processDeath()
{
    if (m_hasProcessedDeath)
    {
        return;
    }

    m_hasProcessedDeath = true;

    playDeathAnimation();
    startDestroyCountdown(m_destroyDelay);
}

void EnemyDeathHandler::playDeathAnimation()
{
    if (m_deathStateName.empty())
    {
        return;
    }

    AnimationComponent* animation = AnimationAPI::getAnimationComponent(m_owner);

    if (animation == nullptr)
    {
        Debug::warn("EnemyDeathHandler on '%s' could not find AnimationComponent.", GameObjectAPI::getName(m_owner));
        return;
    }

    const bool played = AnimationAPI::playState(animation, m_deathStateName.c_str(), 0.0f);

    if (!played)
    {
        Debug::warn("EnemyDeathHandler on '%s' could not play death state '%s'.", GameObjectAPI::getName(m_owner), m_deathStateName.c_str());
    }
}

void EnemyDeathHandler::startDestroyCountdown(float delay)
{
    m_waitingToDestroy = true;
    m_deathTimer = delay;
}

void EnemyDeathHandler::destroyEnemyNow()
{
    m_waitingToDestroy = false;
    GameObjectAPI::removeGameObject(m_owner);
}

void EnemyDeathHandler::DropHealth()
{
    if (m_healthPrefabPath.empty())
    {
        return;
    }

    const Transform* myTransform = GameObjectAPI::getTransform(m_owner);
    if (myTransform == nullptr)
    {
        return;
    }

    const Vector3 spawnPosition = TransformAPI::getGlobalPosition(myTransform);

    for (int i = 0; i < m_healthDropQuantity; ++i)
    {
        
        float angle = (static_cast<float>(rand()) / RAND_MAX) * 6.283185f;

        
        float distance = (static_cast<float>(rand()) / RAND_MAX) * m_dropRadius;

       
        Vector3 offset;
        offset.x = std::cos(angle) * distance;
        offset.z = std::sin(angle) * distance;
        offset.y = 0.0f; 

        Vector3 finalPos = spawnPosition + offset;
        GameObject* pickup = GameObjectAPI::instantiatePrefab(m_healthPrefabPath.c_str(), finalPos, Vector3::Zero);

        if (pickup == nullptr)
        {
            continue;
        }

        Script* script = GameObjectAPI::getScript(pickup, "HealthPickup");
        if (script != nullptr)
        {
            static_cast<HealthPickup*>(script)->m_healAmount = m_healthDropAmount;
        }
    }
}
IMPLEMENT_SCRIPT(EnemyDeathHandler)
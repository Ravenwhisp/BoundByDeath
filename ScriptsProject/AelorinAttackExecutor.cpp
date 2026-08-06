#include "pch.h"
#include "AelorinAttackExecutor.h"

#include "AelorinDetectionAggro.h"
#include "Damageable.h"
#include "PlayerState.h"

#include <cmath>

AelorinAttackExecutor::AelorinAttackExecutor(GameObject* owner)
    : Script(owner)
{
}

void AelorinAttackExecutor::Start()
{
    m_detectionAggro = GameObjectAPI::findScript<AelorinDetectionAggro>(getOwner());

    if (!m_detectionAggro)
    {
        Debug::error("[AelorinAttackExecutor] AelorinDetectionAggro script not found");
    }
}

void AelorinAttackExecutor::applyDamageInRadius(const Vector3& center, float radius, float damage, const char* sourceName)
{
    if (!m_detectionAggro)
    {
        return;
    }

    Transform* lyrielTransform = m_detectionAggro->getLyrielTransform();
    Transform* deathTransform = m_detectionAggro->getDeathTransform();

    tryDamageTargetInRadius(lyrielTransform, center, radius, damage, sourceName);
    tryDamageTargetInRadius(deathTransform, center, radius, damage, sourceName);
}

bool AelorinAttackExecutor::tryDamageTargetInRadius(Transform* targetTransform, const Vector3& center, float radius, float damage, const char* sourceName)
{
    if (!targetTransform)
    {
        return false;
    }

    GameObject* targetObject = ComponentAPI::getOwner(targetTransform);
    if (!targetObject)
    {
        return false;
    }

    Vector3 targetPosition = TransformAPI::getGlobalPosition(targetTransform);

    Vector3 difference = targetPosition - center;
    difference.y = 0.0f;

    const float distanceSquared = difference.LengthSquared();
    const float radiusSquared = radius * radius;

    if (distanceSquared > radiusSquared)
    {
        return false;
    }

    return applyDamageToTarget(targetTransform, damage, sourceName);
}

bool AelorinAttackExecutor::applyDamageToTarget(Transform* targetTransform, float damage, const char* sourceName)
{
    if (!targetTransform)
    {
        return false;
    }

    GameObject* targetObject = ComponentAPI::getOwner(targetTransform);
    if (!targetObject)
    {
        return false;
    }

    Damageable* damageable = GameObjectAPI::findScript<Damageable>(targetObject);
    if (!damageable)
    {
        return false;
    }

    PlayerState* playerState = GameObjectAPI::findScript<PlayerState>(targetObject);
    if (playerState && playerState->isDowned())
    {
        return false;
    }

    damageable->takeDamage(damage);

    Debug::log("[AelorinAttackExecutor] %s damaged '%s' for %.2f.", sourceName, GameObjectAPI::getName(targetObject), damage);
    return true;
}

IMPLEMENT_SCRIPT(AelorinAttackExecutor)
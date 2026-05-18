#include "pch.h"
#include "ArthurAttackExecutor.h"

#include "ArthurDetectionAggro.h"
#include "Damageable.h"

#include <cmath>

ArthurAttackExecutor::ArthurAttackExecutor(GameObject* owner)
    : Script(owner)
{
}

void ArthurAttackExecutor::Start()
{
    m_arthurDetectionAggro = GameObjectAPI::findScript<ArthurDetectionAggro>(getOwner());

    if (!m_arthurDetectionAggro)
    {
        Debug::error("[ArthurAttackExecutor] ArthurDetectionAggro script not found");
    }
}

void ArthurAttackExecutor::applyDamageInRadius(const Vector3& center, float radius, float damage, const char* sourceName)
{
    if (!m_arthurDetectionAggro)
    {
        return;
    }

    Transform* lyrielTransform = m_arthurDetectionAggro->getLyrielTransform();
    Transform* deathTransform = m_arthurDetectionAggro->getDeathTransform();

    tryDamageTargetInRadius(lyrielTransform, center, radius, damage, sourceName);
    tryDamageTargetInRadius(deathTransform, center, radius, damage, sourceName);
}

void ArthurAttackExecutor::applyDamageInCone(const Vector3& center, const Vector3& direction, float range, float halfAngleDegrees, float damage, const char* sourceName)
{
    if (!m_arthurDetectionAggro)
    {
        return;
    }

    Transform* lyrielTransform = m_arthurDetectionAggro->getLyrielTransform();
    Transform* deathTransform = m_arthurDetectionAggro->getDeathTransform();

    tryDamageTargetInCone(lyrielTransform, center, direction, range, halfAngleDegrees, damage, sourceName);
    tryDamageTargetInCone(deathTransform, center, direction, range, halfAngleDegrees, damage, sourceName);
}

void ArthurAttackExecutor::tryDamageTargetInRadius(Transform* targetTransform, const Vector3& center, float radius, float damage, const char* sourceName)
{
    if (!targetTransform)
    {
        return;
    }

    GameObject* targetObject = ComponentAPI::getOwner(targetTransform);
    if (!targetObject)
    {
        return;
    }

    Vector3 targetPosition = TransformAPI::getGlobalPosition(targetTransform);

    Vector3 difference = targetPosition - center;
    difference.y = 0.0f;

    const float distanceSquared = difference.LengthSquared();
    const float radiusSquared = radius * radius;

    if (distanceSquared > radiusSquared)
    {
        return;
    }

    applyDamageToTarget(targetTransform, damage, sourceName);
}

void ArthurAttackExecutor::tryDamageTargetInCone(Transform* targetTransform, const Vector3& center, const Vector3& direction, float range, float halfAngleDegrees, float damage, const char* sourceName)
{
    if (!targetTransform)
    {
        return;
    }

    GameObject* targetObject = ComponentAPI::getOwner(targetTransform);
    if (!targetObject)
    {
        return;
    }

    Vector3 targetPosition = TransformAPI::getGlobalPosition(targetTransform);

    Vector3 toTarget = targetPosition - center;
    toTarget.y = 0.0f;

    const float distanceSquared = toTarget.LengthSquared();
    const float rangeSquared = range * range;

    if (distanceSquared > rangeSquared)
    {
        return;
    }

    if (distanceSquared < 0.0001f)
    {
        applyDamageToTarget(targetTransform, damage, sourceName);
        return;
    }

    Vector3 flatDirection = direction;
    flatDirection.y = 0.0f;

    if (flatDirection.LengthSquared() < 0.0001f)
    {
        return;
    }

    toTarget.Normalize();
    flatDirection.Normalize();

    float dot = flatDirection.Dot(toTarget);

    if (dot > 1.0f)
    {
        dot = 1.0f;
    }
    else if (dot < -1.0f)
    {
        dot = -1.0f;
    }

    constexpr float degreesToRadians = 3.14159265f / 180.0f;
    const float minDot = std::cos(halfAngleDegrees * degreesToRadians);

    if (dot < minDot)
    {
        return;
    }

    applyDamageToTarget(targetTransform, damage, sourceName);
}

void ArthurAttackExecutor::applyDamageToTarget(Transform* targetTransform, float damage, const char* sourceName)
{
    if (!targetTransform)
    {
        return;
    }

    GameObject* targetObject = ComponentAPI::getOwner(targetTransform);
    if (!targetObject)
    {
        return;
    }

    Damageable* damageable = GameObjectAPI::findScript<Damageable>(targetObject);
    if (!damageable)
    {
        return;
    }

    if (damageable->isDead())
    {
        return;
    }

    damageable->takeDamage(damage);

    Debug::log("[ArthurAttackExecutor] %s damaged '%s' for %.2f.", sourceName, GameObjectAPI::getName(targetObject), damage);
}

IMPLEMENT_SCRIPT(ArthurAttackExecutor)
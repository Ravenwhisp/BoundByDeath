#include "pch.h"
#include "EnemyBaseController.h"

#include "Damageable.h"

EnemyBaseController::EnemyBaseController(GameObject* owner)
    : Script(owner)
{
}

void EnemyBaseController::updateCurrentTarget()
{
    Transform* previousTarget = m_currentTarget;

    m_currentTarget = acquireCurrentTarget();

    if (m_currentTarget != previousTarget)
    {
        clearPath();
        resetRepathTimer();
    }
}

bool EnemyBaseController::hasValidTarget() const
{
    return m_currentTarget != nullptr;
}

float EnemyBaseController::getDistanceToCurrentTarget() const
{
    if (!m_currentTarget)
    {
        return FLT_MAX;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());

    if (!ownerTransform)
    {
        return FLT_MAX;
    }

    Vector3 ownerPosition = TransformAPI::getPosition(ownerTransform);
    Vector3 targetPosition = TransformAPI::getPosition(m_currentTarget);

    Vector3 difference = targetPosition - ownerPosition;
    difference.y = 0.0f;

    return difference.Length();
}

bool EnemyBaseController::isCurrentTargetInRange(float range) const
{
    if (!hasValidTarget())
    {
        return false;
    }

    return getDistanceToCurrentTarget() <= range;
}

void EnemyBaseController::faceCurrentTarget()
{
    if (!m_currentTarget)
    {
        return;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());

    if (!ownerTransform)
    {
        return;
    }

    Vector3 ownerPosition = TransformAPI::getPosition(ownerTransform);
    Vector3 targetPosition = TransformAPI::getPosition(m_currentTarget);

    Vector3 direction = targetPosition - ownerPosition;
    direction.y = 0.0f;

    if (direction.LengthSquared() <= 0.00001f)
    {
        return;
    }

    rotateTowardsDirection(direction);
}

void EnemyBaseController::facePosition(const Vector3& worldPosition)
{
    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());

    if (!ownerTransform)
    {
        return;
    }

    Vector3 ownerPosition = TransformAPI::getPosition(ownerTransform);

    Vector3 direction = worldPosition - ownerPosition;
    direction.y = 0.0f;

    if (direction.LengthSquared() <= 0.00001f)
    {
        return;
    }

    rotateTowardsDirection(direction);
}

void EnemyBaseController::rotateTowardsDirection(const Vector3& direction)
{
    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());

    if (!ownerTransform)
    {
        return;
    }

    Vector3 desiredDirection = direction;
    desiredDirection.y = 0.0f;

    if (desiredDirection.LengthSquared() <= 0.00001f)
    {
        return;
    }

    desiredDirection.Normalize();

    Vector3 currentEuler = TransformAPI::getEulerDegrees(ownerTransform);

    constexpr float radiansToDegrees = 180.0f / 3.14159265f;

    const float desiredYawRadians = std::atan2(desiredDirection.x, desiredDirection.z);
    const float desiredYawDegrees = desiredYawRadians * radiansToDegrees;

    float deltaYaw = desiredYawDegrees - currentEuler.y;

    while (deltaYaw > 180.0f)
    {
        deltaYaw -= 360.0f;
    }

    while (deltaYaw < -180.0f)
    {
        deltaYaw += 360.0f;
    }

    const float maxStep = getTurnSpeedDegrees() * Time::getDeltaTime();

    if (deltaYaw > maxStep)
    {
        deltaYaw = maxStep;
    }

    if (deltaYaw < -maxStep)
    {
        deltaYaw = -maxStep;
    }

    currentEuler.y += deltaYaw;

    TransformAPI::setRotationEuler(ownerTransform, currentEuler);
}

bool EnemyBaseController::isDead() const
{
    Damageable* damageable = GameObjectAPI::findScript<Damageable>(getOwner());

    if (!damageable)
    {
        return false;
    }

    return damageable->isDead();
}

bool EnemyBaseController::trySendDeathTrigger(AnimationComponent* animation)
{
    if (m_deathTriggerSent)
    {
        return false;
    }

    if (!isDead())
    {
        return false;
    }

    if (!animation)
    {
        return false;
    }

    clearPath();

    const bool sent = AnimationAPI::sendTrigger(animation, "ToDeath");

    if (!sent)
    {
        return false;
    }

    m_deathTriggerSent = true;

    Debug::log("[EnemyBaseController] ToDeath trigger sent.");

    return true;
}
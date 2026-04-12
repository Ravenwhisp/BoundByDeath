#include "pch.h"
#include "LyrielArrowProjectile.h"
#include "ArrowPool.h"

LyrielArrowProjectile::LyrielArrowProjectile(GameObject* owner)
    : Script(owner)
{
}

void LyrielArrowProjectile::setPool(ArrowPool* pool)
{
    m_pool = pool;
}

bool LyrielArrowProjectile::isInUse() const
{
    return m_inUse;
}

void LyrielArrowProjectile::launch(const Vector3& start_position, const Vector3& direction, float speed, float lifetime)
{
    m_inUse = true;
    m_direction = direction;
    m_direction.Normalize();
    m_speed = speed;
    m_lifeTimer = 0.0f;
    m_currentLifetime = lifetime;

    Transform* transform = GameObjectAPI::getTransform(getOwner());
    if (transform != nullptr)
    {
        TransformAPI::setGlobalPosition(transform, start_position);
        orientToDirection(transform, m_direction);
    }

    GameObjectAPI::setActive(getOwner(), true);
}

void LyrielArrowProjectile::Update()
{
    if (!m_inUse)
    {
        return;
    }

    m_lifeTimer += Time::getDeltaTime();

    Transform* transform = GameObjectAPI::getTransform(getOwner());
    if (transform != nullptr)
    {
        TransformAPI::translateGlobal(transform, m_direction * m_speed * Time::getDeltaTime());
    }

    if (m_lifeTimer >= m_currentLifetime)
    {
        returnToPool();
    }
}

void LyrielArrowProjectile::resetProjectile()
{
    m_inUse = false;
    m_direction = Vector3::Zero;
    m_speed = 0.0f;
    m_lifeTimer = 0.0f;
    m_currentLifetime = 0.0f;

    GameObjectAPI::setActive(getOwner(), false);
}

void LyrielArrowProjectile::returnToPool()
{
    if (m_pool == nullptr)
    {
        resetProjectile();
        return;
    }

    m_pool->releaseArrow(this);
}

void LyrielArrowProjectile::orientToDirection(Transform* transform, const Vector3& direction)
{
    if (transform == nullptr)
    {
        return;
    }

    Vector3 dir = direction;
    if (dir.LengthSquared() <= 0.0001f)
    {
        return;
    }

    dir.Normalize();

    const float yawRadians = atan2f(dir.x, dir.z);
    const float pitchRadians = -asinf(dir.y);

    const float radToDeg = 57.2957795f;
    Vector3 eulerDegrees(pitchRadians * radToDeg, yawRadians * radToDeg, 0.0f);

    TransformAPI::setRotationEuler(transform, eulerDegrees);
}

IMPLEMENT_SCRIPT(LyrielArrowProjectile)
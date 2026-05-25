#include "pch.h"
#include "LyrielArrowProjectile.h"
#include "ArrowPool.h"
#include "EnemyDamageable.h"
#include "BreakableDamageable.h"
#include "EnemyShadowMark.h"
#include "LyrielCharacter.h"
#include "LyrielSound.h"

LyrielArrowProjectile::LyrielArrowProjectile(GameObject* owner)
    : Script(owner)
{
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
        applyImpactDamage();
        returnToPool();
    }
}

void LyrielArrowProjectile::setPool(ArrowPool* pool)
{
    m_pool = pool;
}

void LyrielArrowProjectile::setArrowOwnerTransform(Transform* owner)
{
    m_arrowOwner = owner;
}

bool LyrielArrowProjectile::isInUse() const
{
    return m_inUse;
}

void LyrielArrowProjectile::launch(const Vector3& start_position, const Vector3& direction, float speed, float lifetime, GameObject* target, float damage)
{
    m_inUse = true;
    m_direction = direction;
    m_direction.Normalize();
    m_speed = speed;
    m_lifeTimer = 0.0f;
    m_currentLifetime = lifetime;
    m_target = target;
    m_damage = damage;

    Transform* transform = GameObjectAPI::getTransform(getOwner());
    if (transform != nullptr)
    {
        TransformAPI::setGlobalPosition(transform, start_position);
        TransformAPI::lookAt(transform, start_position + m_direction);
    }

    GameObjectAPI::setActive(getOwner(), true);
}

void LyrielArrowProjectile::resetProjectile()
{
    m_inUse = false;
    m_direction = Vector3::Zero;
    m_speed = 0.0f;
    m_lifeTimer = 0.0f;
    m_currentLifetime = 0.0f;
    m_target = nullptr;
    m_damage = 0.0f;

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

void LyrielArrowProjectile::applyImpactDamage()
{
    if (m_target == nullptr)
    {
        return;
    }

    // Resolve LyrielSound on the shooter once for both impact + mark exploit feedback.
    LyrielSound* sound = nullptr;
    if (m_arrowOwner != nullptr)
    {
        GameObject* shooter = m_arrowOwner->getOwner();
        if (shooter != nullptr)
        {
            sound = GameObjectAPI::findScript<LyrielSound>(shooter);
        }
    }

    if (sound != nullptr)
    {
        sound->playArrowImpact();
    }

    EnemyDamageable* damageable = GameObjectAPI::findScript<EnemyDamageable>(m_target);

    if (damageable != nullptr)
    {
        {
            EnemyHitContext ctx;
            ctx.damage = m_damage;
            ctx.attacker = m_arrowOwner;
            ctx.attackType = EnemyAttackType::LyrielArrow;
            damageable->takeDamage(ctx);
        }

        EnemyShadowMark* mark = GameObjectAPI::findScript<EnemyShadowMark>(m_target);
        if (mark != nullptr && mark->isExploitable())
        {
            mark->exploit();

            if (sound != nullptr)
            {
                sound->playMarkExploit();
            }

            if (m_arrowOwner != nullptr)
            {
                GameObject* shooter = m_arrowOwner->getOwner();
                if (shooter != nullptr)
                {
                    LyrielCharacter* lyriel = GameObjectAPI::findScript<LyrielCharacter>(shooter);
                    if (lyriel != nullptr)
                        lyriel->onMarkExploited();
                }
            }
        }
    }

	BreakableDamageable* breakableDamageable = GameObjectAPI::findScript<BreakableDamageable>(m_target);

    if (breakableDamageable != nullptr)
    {
        breakableDamageable->takeDamage(m_damage);
        return;
    }
}

IMPLEMENT_SCRIPT(LyrielArrowProjectile)

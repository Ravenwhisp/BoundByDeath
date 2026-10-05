#include "pch.h"
#include "LyrielArrowProjectile.h"
#include "EnemyDamageable.h"
#include "BreakableDamageable.h"
#include "LyrielCharacter.h"
#include "LyrielSound.h"
#include "ParticleLifecycle.h"

IMPLEMENT_SCRIPT_FIELDS(LyrielArrowProjectile,
    SERIALIZED_ASSET_REF(m_visualChargedPrefab, "Visual Charged Prefab", AssetType::PREFAB),
    SERIALIZED_ASSET_REF(m_visualVolleyPrefab, "Visual Volley Prefab", AssetType::PREFAB)
)

LyrielArrowProjectile::LyrielArrowProjectile(GameObject* owner)
    : ProjectileBase(owner)
{
}

void LyrielArrowProjectile::Update()
{
    if (!m_inUse)
    {
        return;
    }

    m_lifeTimer += Time::getDeltaTime();

    if (m_transform != nullptr)
    {
        TransformAPI::translateGlobal(m_transform, m_direction * m_speed * Time::getDeltaTime());
    }

    if (m_lifeTimer >= m_currentLifetime)
    {
        applyImpactDamage();
        returnToPool();
    }
}

void LyrielArrowProjectile::launch(const Vector3& startPosition, const Vector3& direction, float speed, float lifetime, GameObject* target, float damage, VisualModel visual)
{
    m_direction = direction;
    m_speed = speed;
    m_currentLifetime = lifetime;
    m_lifeTimer = 0.0f;

    m_target = target;
    m_damage = damage;

    prepareVisuals();
    cacheShooterScripts();

    if (m_transform != nullptr)
    {
        TransformAPI::setGlobalPosition(m_transform, startPosition);
        TransformAPI::lookAt(m_transform, startPosition + direction);

        Vector3 euler = TransformAPI::getGlobalEulerDegrees(m_transform);
        euler.y += 180.0f;
        if (euler.y > 180.0f)
        {
            euler.y -= 360.0f;
        }
        TransformAPI::setGlobalRotationEuler(m_transform, euler);
    }

    m_inUse = true;

    GameObjectAPI::setActive(getOwner(), true);
    activateVisual(visual);
}

void LyrielArrowProjectile::resetProjectile()
{
    prepareVisuals();
    deactivateVisuals();

    GameObjectAPI::setActive(getOwner(), false);

    m_direction = Vector3::Zero;
    m_speed = 0.0f;
    m_currentLifetime = 0.0f;
    m_lifeTimer = 0.0f;

    m_target = nullptr;
    m_damage = 0.0f;

    m_inUse = false;
}

void LyrielArrowProjectile::applyImpactDamage()
{
    if (m_target == nullptr)
    {
        return;
    }

    Transform* projectileOwner = getProjectileOwnerTransform();

    if (m_sound != nullptr)
    {
        m_sound->playArrowImpact();
    }

    EnemyDamageable* damageable = GameObjectAPI::findScript<EnemyDamageable>(m_target);

    if (damageable != nullptr)
    {
        EnemyHitContext ctx;
        ctx.damage = m_damage;
        ctx.attacker = projectileOwner;
        ctx.attackType = PlayerAttackType::LyrielArrow;

        damageable->takeDamage(ctx);
        if (damageable->lastHitExploitShadowMark() && projectileOwner != nullptr)
        {
            GameObject* shooter = projectileOwner->getOwner();

            if (shooter != nullptr)
            {
                if (m_lyrielCharacter != nullptr)
                {
                    m_lyrielCharacter->onMarkExploited();
                }
            }
        }

        return;
    }

	BreakableDamageable* breakableDamageable = GameObjectAPI::findScript<BreakableDamageable>(m_target);

    if (breakableDamageable != nullptr)
    {
        breakableDamageable->takeDamage(m_damage);
    }
}

void LyrielArrowProjectile::prepareVisuals()
{
    if (m_visualsPrepared)
    {
        return;
    }

    m_transform = GameObjectAPI::getTransform(getOwner());
    m_basicModel = GameObjectAPI::getComponent(getOwner(), ComponentType::MODEL);
    m_basicTrail = TrailAPI::getTrailComponent(getOwner());
    if (m_transform != nullptr)
    {
        if (Transform* adornment = TransformAPI::findChildByName(m_transform, "ParticleLyrielArrow"))
        {
            m_basicAdornment = ComponentAPI::getOwner(adornment);
        }
    }

    if (m_visualChargedPrefab.m_id.isValid())
    {
        m_chargedVisual = GameObjectAPI::instantiatePrefab(
            m_visualChargedPrefab.m_id, Vector3::Zero, Vector3::Zero, getOwner());
        m_chargedTrail = TrailAPI::getTrailComponent(m_chargedVisual);
    }

    if (m_visualVolleyPrefab.m_id.isValid())
    {
        m_volleyVisual = GameObjectAPI::instantiatePrefab(
            m_visualVolleyPrefab.m_id, Vector3::Zero, Vector3::Zero, getOwner());
        m_volleyTrail = TrailAPI::getTrailComponent(m_volleyVisual);
    }

    m_visualsPrepared = true;
    deactivateVisuals();
}

void LyrielArrowProjectile::cacheShooterScripts()
{
    if (m_sound != nullptr && m_lyrielCharacter != nullptr)
    {
        return;
    }

    Transform* projectileOwner = getProjectileOwnerTransform();
    GameObject* shooter = projectileOwner != nullptr ? projectileOwner->getOwner() : nullptr;
    if (shooter != nullptr)
    {
        m_sound = GameObjectAPI::findScript<LyrielSound>(shooter);
        m_lyrielCharacter = GameObjectAPI::findScript<LyrielCharacter>(shooter);
    }
}

void LyrielArrowProjectile::setBasicVisualActive(bool active)
{
    if (m_basicModel != nullptr)
    {
        ComponentAPI::setActive(m_basicModel, active);
    }
    if (m_basicAdornment != nullptr)
    {
        GameObjectAPI::setActive(m_basicAdornment, active);
    }
    if (m_basicTrail != nullptr)
    {
        if (active)
        {
            TrailAPI::clearTrail(m_basicTrail);
            TrailAPI::generateTrail(m_basicTrail, true);
        }
        else
        {
            TrailAPI::generateTrail(m_basicTrail, false);
            TrailAPI::clearTrail(m_basicTrail);
        }
    }
}

void LyrielArrowProjectile::setExternalVisualActive(GameObject* visualObject, TrailComponent* trail, bool active)
{
    if (visualObject == nullptr)
    {
        return;
    }

    if (active)
    {
        GameObjectAPI::setActive(visualObject, true);
        if (trail != nullptr)
        {
            TrailAPI::clearTrail(trail);
            TrailAPI::generateTrail(trail, true);
        }
        ParticleLifecycle::restart(visualObject);
    }
    else
    {
        ParticleLifecycle::stop(visualObject);
        if (trail != nullptr)
        {
            TrailAPI::generateTrail(trail, false);
            TrailAPI::clearTrail(trail);
        }
        GameObjectAPI::setActive(visualObject, false);
    }
}

void LyrielArrowProjectile::deactivateVisuals()
{
    setBasicVisualActive(false);
    setExternalVisualActive(m_chargedVisual, m_chargedTrail, false);
    setExternalVisualActive(m_volleyVisual, m_volleyTrail, false);
}

void LyrielArrowProjectile::activateVisual(VisualModel visual)
{
    deactivateVisuals();
    switch (visual)
    {
    case VisualModel::Basic:
        setBasicVisualActive(true);
        break;
    case VisualModel::Charged:
        setExternalVisualActive(m_chargedVisual, m_chargedTrail, true);
        break;
    case VisualModel::Volley:
        setExternalVisualActive(m_volleyVisual, m_volleyTrail, true);
        break;
    }
}

IMPLEMENT_SCRIPT(LyrielArrowProjectile)

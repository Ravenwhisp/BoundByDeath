#include "pch.h"
#include "BarrierEnemyDamageable.h"
#include "Transform2D.h"
#include <algorithm>

IMPLEMENT_SCRIPT_FIELDS_INHERITED(BarrierEnemyDamageable, EnemyDamageable,
    SERIALIZED_FLOAT_VECTOR(m_barriersThresholds, "Barrier Thresholds (%)"),
    SERIALIZED_ASSET_REF(m_barrierPrefab, "Barrier UI Prefab", AssetType::PREFAB),
    SERIALIZED_FLOAT(m_minPos, "Barrier Min Pos (0% HP)", -1000.0f, 1000.0f, 1.0f),
    SERIALIZED_FLOAT(m_maxPos, "Barrier Max Pos (100% HP)", -1000.0f, 1000.0f, 1.0f),
    SERIALIZED_FLOAT(m_barrierUIHeight, "Barrier UI Height", -1000.0f, 1000.0f, 1.0f)
)

BarrierEnemyDamageable::BarrierEnemyDamageable(GameObject* owner)
    : EnemyDamageable(owner)
{
}

void BarrierEnemyDamageable::Start()
{
    EnemyDamageable::Start();
    buildBarriers();
    instantiateBarrierUIs();
}

void BarrierEnemyDamageable::buildBarriers()
{
    m_barriers.clear();
    m_nextBarrierIndex = 0;

    for (float threshold : m_barriersThresholds)
    {
        float pct = std::clamp(threshold, 0.0f, 1.0f);
        if (pct > 0.0f)
        {
            Barrier b;
            b.hpPercent = pct;
            b.broken = false;
            m_barriers.push_back(b);
        }
    }

    std::sort(m_barriers.begin(), m_barriers.end(),
        [](const Barrier& a, const Barrier& b) { return a.hpPercent > b.hpPercent; });
}

void BarrierEnemyDamageable::instantiateBarrierUIs()
{
    m_barrierUIs.clear();

    if (!m_barrierPrefab.m_ref.isValid())
        return;

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    Transform* healthBarTransform = TransformAPI::findChildByName(ownerTransform, "Health Bar");
    if (!healthBarTransform)
    {
        Debug::warn("[Barrier] %s - Health Bar not found in hierarchy.", GameObjectAPI::getName(m_owner));
        return;
    }

    GameObject* healthBarObject = ComponentAPI::getOwner(healthBarTransform);

    for (const Barrier& barrier : m_barriers)
    {
        GameObject* uiObject = GameObjectAPI::instantiatePrefab(
            m_barrierPrefab.m_ref,
            Vector3::Zero,
            Vector3::Zero,
            healthBarObject);

        if (!uiObject)
        {
            Debug::warn("[Barrier] %s - Failed to instantiate barrier UI.", GameObjectAPI::getName(m_owner));
            continue;
        }

        TransformAPI::setPosition(GameObjectAPI::getTransform(uiObject), Vector3::Zero);

        Transform2D* transform2D = static_cast<Transform2D*>(GameObjectAPI::getComponent(uiObject, ComponentType::TRANSFORM2D));
        if (transform2D)
        {
            float x = m_maxPos + (1.0f - barrier.hpPercent) * (m_minPos - m_maxPos);
            Transform2DAPI::setPosition(transform2D, { x, m_barrierUIHeight });
        }

        BarrierUI ui;
        ui.gameObject = uiObject;
        ui.hpPercent = barrier.hpPercent;
        m_barrierUIs.push_back(ui);
    }
}

void BarrierEnemyDamageable::destroyBrokenBarrierUI(size_t index)
{
    if (index >= m_barrierUIs.size())
        return;

    BarrierUI& ui = m_barrierUIs[index];
    if (ui.gameObject)
    {
        GameObjectAPI::removeGameObject(ui.gameObject);
        ui.gameObject = nullptr;
    }
}

float BarrierEnemyDamageable::getNextBarrierAbsoluteHp() const
{
    for (size_t i = 0; i < m_barriers.size(); ++i)
    {
        if (!m_barriers[i].broken)
        {
            return m_maxHp * m_barriers[i].hpPercent;
        }
    }
    return 0.0f;
}

void BarrierEnemyDamageable::takeDamage(float amount)
{
    EnemyHitContext hit;
    hit.damage = amount;
    hit.attackType = PlayerAttackType::Environment;
    takeDamage(hit);
}

void BarrierEnemyDamageable::breakNextBarrier()
{
    for (size_t i = 0; i < m_barriers.size(); ++i)
    {
        if (m_barriers[i].broken)
        {
            continue;
        }

        m_barriers[i].broken = true;
        m_nextBarrierIndex = i + 1;
        destroyBrokenBarrierUI(i);

        Debug::log("[Barrier] %s broke barrier at %.0f%% HP through Shadow Mark exploit.", GameObjectAPI::getName(m_owner), m_barriers[i].hpPercent * 100.0f);

        return;
    }
}

void BarrierEnemyDamageable::takeDamage(const HitContext& ctx)
{
    const EnemyHitContext& hit = static_cast<const EnemyHitContext&>(ctx);

    resetLastShadowMarkResult();

    if (m_isDead || m_invulnerable)
    {
        return;
    }

    const bool shadowMarkExploited = processShadowMarkHit(hit.attackType);
    float nextBarrierHp = getNextBarrierAbsoluteHp();

    if (nextBarrierHp <= 0.0f)
    {
        applyDamageWithoutShadowMark(hit);
        return;
    }

    const float hpAfter = m_currentHp - hit.damage;

    // the hit does not reach the barrier
    if (hpAfter >= nextBarrierHp)
    {
        applyDamageWithoutShadowMark(hit);
        return;
    }

    // shadow mark exploit breaks thebarrier but deals no damage
    if (shadowMarkExploited)
    {
        breakNextBarrier();
        return;
    }

    // any other hit can only deal damage up to the barrier
    const float allowedDamage = m_currentHp - nextBarrierHp;

    if (allowedDamage > 0.0f)
    {
        EnemyHitContext limitedHit = hit;
        limitedHit.damage = allowedDamage;
        applyDamageWithoutShadowMark(limitedHit);
    }

    Debug::log("[Barrier] %s blocked hit at %.0f%% HP.", GameObjectAPI::getName(m_owner), (nextBarrierHp / m_maxHp) * 100.0f);
}

void BarrierEnemyDamageable::kill()
{
    if (hasActiveBarriers())
    {
        Debug::log("[Barrier] %s kill prevented by active barrier.", GameObjectAPI::getName(m_owner));
        return;
    }

    EnemyDamageable::kill();
}

IMPLEMENT_SCRIPT(BarrierEnemyDamageable)

#include "pch.h"
#include "BarrierEnemyDamageable.h"

#include <sstream>
#include <algorithm>
#include <cmath>

static const char* barrierAttackTypeNames[] =
{
    "None",
    "DeathBasic",
    "DeathCharged",
    "DeathDash",
    "LyrielArrow",
    "LyrielVolley",
    "LyrielCharged",
    "ShadowExecution",
    "Environment"
};

constexpr int barrierAttackTypeCount = 9;

IMPLEMENT_SCRIPT_FIELDS_INHERITED(BarrierEnemyDamageable, EnemyDamageable, 
    SERIALIZED_STRING(m_barrierPercentagesStr, "Barrier Thresholds (%)"),
    SERIALIZED_ENUM_INT(m_requiredAttackType, "Barrier Break Attack", barrierAttackTypeNames, barrierAttackTypeCount),
    SERIALIZED_BOOL(m_shadowExecutionBreaksBarriers, "Shadow Execution Breaks Barriers")
)

BarrierEnemyDamageable::BarrierEnemyDamageable(GameObject* owner)
    : EnemyDamageable(owner)
{
}

void BarrierEnemyDamageable::Start()
{
    EnemyDamageable::Start();
    parseBarrierConfig();
}

void BarrierEnemyDamageable::parseBarrierConfig()
{
    m_barriers.clear();
    m_nextBarrierIndex = 0;

    if (m_barrierPercentagesStr.empty())
        return;

    std::stringstream ss(m_barrierPercentagesStr);
    std::string token;

    while (std::getline(ss, token, ','))
    {
        float pct = static_cast<float>(std::atof(token.c_str())) / 100.0f;
        pct = std::clamp(pct, 0.0f, 1.0f);

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

    Debug::log("[Barrier] %s parsed %zu barriers: %s",
        GameObjectAPI::getName(m_owner),
        m_barriers.size(),
        m_barrierPercentagesStr.c_str());
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
    hit.attackType = EnemyAttackType::Environment;

    applyHit(hit);
}

bool BarrierEnemyDamageable::canBreakBarrier(EnemyAttackType attackType) const
{
    if (attackType == EnemyAttackType::ShadowExecution)
    {
        return m_shadowExecutionBreaksBarriers;
    }

    return attackType == static_cast<EnemyAttackType>(m_requiredAttackType);
}

void BarrierEnemyDamageable::breakBarrierAtIndex(size_t barrierIndex, float hpBefore, float hpAfter)
{
    if (barrierIndex >= m_barriers.size())
    {
        return;
    }

    m_barriers[barrierIndex].broken = true;
    m_nextBarrierIndex = barrierIndex + 1;

    Debug::log("[Barrier] %s broke barrier at %.0f%% HP (%s, HP: %.1f -> %.1f)",
        GameObjectAPI::getName(m_owner),
        m_barriers[barrierIndex].hpPercent * 100.0f,
        barrierAttackTypeNames[static_cast<int>(m_requiredAttackType)],
        hpBefore,
        hpAfter);
}

void BarrierEnemyDamageable::applyHit(const EnemyHitContext& hit)
{
    if (m_isDead || m_invulnerable || hit.damage <= 0.0f)
    {
        return;
    }

    float nextBarrierHp = getNextBarrierAbsoluteHp();

    if (nextBarrierHp <= 0.0f)
    {
        EnemyDamageable::applyHit(hit);
        return;
    }

    const float hpBefore = m_currentHp;
    const float hpAfter = m_currentHp - hit.damage;

    if (hpAfter >= nextBarrierHp)
    {
        EnemyDamageable::applyHit(hit);
        return;
    }

    if (!canBreakBarrier(hit.attackType))
    {
        const float allowedDamage = m_currentHp - nextBarrierHp;
        if (allowedDamage > 0.0f)
        {
            EnemyHitContext limitedHit = hit;
            limitedHit.damage = allowedDamage;
            EnemyDamageable::applyHit(limitedHit);
        }

        Debug::log("[Barrier] %s blocked hit at %.0f%% HP. Required attack: %s",
            GameObjectAPI::getName(m_owner),
            (nextBarrierHp / m_maxHp) * 100.0f,
            barrierAttackTypeNames[static_cast<int>(m_requiredAttackType)]);
        return;
    }

    // Break barriers BEFORE applying damage so kill() sees them as broken
    for (size_t i = 0; i < m_barriers.size(); ++i)
    {
        if (m_barriers[i].broken)
        {
            continue;
        }

        const float barrierHp = m_maxHp * m_barriers[i].hpPercent;
        if (hpAfter <= barrierHp && hpBefore >= barrierHp)
        {
            m_barriers[i].broken = true;
            m_nextBarrierIndex = i + 1;

            Debug::log("[Barrier] %s broke barrier at %.0f%% HP (%s, HP: %.1f -> %.1f)",
                GameObjectAPI::getName(m_owner),
                m_barriers[i].hpPercent * 100.0f,
                barrierAttackTypeNames[static_cast<int>(m_requiredAttackType)],
                hpBefore,
                hpAfter);
        }
    }

    EnemyDamageable::applyHit(hit);
}

void BarrierEnemyDamageable::kill()
{
    if (hasActiveBarriers())
    {
        Debug::log("[Barrier] %s kill prevented by active barrier.",
            GameObjectAPI::getName(m_owner));
        return;
    }

    EnemyDamageable::kill();
}

IMPLEMENT_SCRIPT(BarrierEnemyDamageable)

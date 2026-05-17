#include "pch.h"
#include "BarrierEnemyDamageable.h"

#include <sstream>
#include <algorithm>
#include <cmath>

IMPLEMENT_SCRIPT_FIELDS(BarrierEnemyDamageable,
    SERIALIZED_STRING(m_barrierPercentagesStr, "Barrier Thresholds (%)"),
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
    if (m_isDead || m_invulnerable || amount <= 0.0f)
        return;

    float nextBarrierHp = getNextBarrierAbsoluteHp();

    if (m_isGaugeExecution)
    {
        float hpBefore = m_currentHp;
        EnemyDamageable::takeDamage(amount);

        for (size_t i = 0; i < m_barriers.size(); ++i)
        {
            if (!m_barriers[i].broken)
            {
                float barrierHp = m_maxHp * m_barriers[i].hpPercent;
                if (m_currentHp <= barrierHp && hpBefore > barrierHp)
                {
                    m_barriers[i].broken = true;
                    m_nextBarrierIndex = i + 1;
                    Debug::log("[Barrier] %s broke barrier at %.0f%% HP (HP: %.1f -> %.1f)",
                        GameObjectAPI::getName(m_owner),
                        m_barriers[i].hpPercent * 100.0f,
                        hpBefore, m_currentHp);
                }
            }
        }
    }
    else
    {
        if (nextBarrierHp > 0.0f)
        {
            float hpAfter = m_currentHp - amount;
            if (hpAfter < nextBarrierHp)
            {
                float allowed = m_currentHp - nextBarrierHp;
                if (allowed > 0.0f)
                {
                    EnemyDamageable::takeDamage(allowed);
                }
                return;
            }
        }

        EnemyDamageable::takeDamage(amount);
    }
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

#pragma once

#include "EnemyDamageable.h"
#include <vector>
#include <string>

class BarrierEnemyDamageable : public EnemyDamageable
{
    DECLARE_SCRIPT(BarrierEnemyDamageable)

public:
    explicit BarrierEnemyDamageable(GameObject* owner);

    void Start() override;
    ScriptFieldList getExposedFields() const override;

    void takeDamage(float amount) override;
    void kill() override;

    bool hasActiveBarriers() const { return m_nextBarrierIndex < m_barriers.size(); }
    size_t getRemainingBarrierCount() const { return m_barriers.size() - m_nextBarrierIndex; }

public:
    std::string m_barrierPercentagesStr = "80";

private:
    struct Barrier
    {
        float hpPercent;
        bool broken;
    };

    void parseBarrierConfig();
    float getNextBarrierAbsoluteHp() const;

    std::vector<Barrier> m_barriers;
    size_t m_nextBarrierIndex = 0;
};

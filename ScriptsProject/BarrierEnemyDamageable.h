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
    void applyHit(const EnemyHitContext& hit) override;
    void kill() override;

    bool hasActiveBarriers() const { return m_nextBarrierIndex < m_barriers.size(); }
    size_t getRemainingBarrierCount() const { return m_barriers.size() - m_nextBarrierIndex; }

public:
    std::string m_barrierPercentagesStr = "80";
    int m_requiredAttackType = static_cast<int>(EnemyAttackType::ShadowExecution);
    bool m_shadowExecutionBreaksBarriers = true;

private:
    struct Barrier
    {
        float hpPercent;
        bool broken;
    };

    void parseBarrierConfig();
    float getNextBarrierAbsoluteHp() const;
    bool canBreakBarrier(EnemyAttackType attackType) const;
    void breakBarrierAtIndex(size_t barrierIndex, float hpBefore, float hpAfter);

    std::vector<Barrier> m_barriers;
    size_t m_nextBarrierIndex = 0;
};

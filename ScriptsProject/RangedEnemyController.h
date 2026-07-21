#pragma once

#include "EnemyBaseController.h"
#include "Timer.h"

class EnemyDetectionAggro;
class ArcherAttackConfig;
class Transform;

class RangedEnemyController : public EnemyBaseController
{
    DECLARE_SCRIPT(RangedEnemyController)

public:
    explicit RangedEnemyController(GameObject* owner);

    void Start() override;
    void Update() override;
    FieldList getExposedFields() const override;

    const EnemyBaseAttackConfig* getAttackConfig() const override;

    // Somersault helpers
    bool playerInSomersaultRange() const;

    bool isSomersaultReady() const;
    void consumeSomersaultCooldown();

    Vector3 getDirectionAwayFromClosestPlayer() const;

    // Arrow Barrage helpers
    bool isArrowBarrageReady() const;
    void consumeArrowBarrageCooldown();

    bool isTargetInArrowBarrageRange() const;

    // Post-somersault range buff
    void activateSomersaultRangeBuff();
    bool isSomersaultRangeBuffActive() const;
    bool isTargetInAttackRange() const;

protected:
    Transform* acquireCurrentTarget() override;
    bool isTargetDowned(Transform* target) const override;

private:
    EnemyDetectionAggro* m_enemyDetectionAggro = nullptr;

    Timer m_somersaultCooldownTimer;
    Timer m_arrowBarrageCooldownTimer;
    Timer m_basicAttackRangeBuffTimer;

public:
    AssetReference<ArcherAttackConfig> m_attackConfig;
};
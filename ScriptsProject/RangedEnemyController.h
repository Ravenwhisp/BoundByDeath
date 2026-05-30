#pragma once

#include "EnemyBaseController.h"

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

    ScriptFieldList getExposedFields() const override;

    bool isTargetInAttackRange() const;

    bool moveTowardsTarget();

    // Somersault helpers
    bool playerInSomersaultRange() const;

    bool isSomersaultReady() const;
    void consumeSomersaultCooldown();
    void updateSomersaultCooldown(float dt);

    Vector3 getDirectionAwayFromClosestPlayer() const;

    // Arrow Barrage helpers
    bool isArrowBarrageReady() const;
    void consumeArrowBarrageCooldown();
    void updateArrowBarrageCooldown(float dt);

    bool isTargetInArrowBarrageRange() const;

protected:
    Transform* acquireCurrentTarget() override;
    float getTurnSpeedDegrees() const override;

private:
    bool rebuildPathToTarget();

public:
    float m_moveSpeed = 3.5f;
    float m_pathPointReachDistance = 0.25f;
    float m_repathInterval = 0.5f;
    float m_turnSpeedDegrees = 360.0f;

private:
    EnemyDetectionAggro* m_enemyDetectionAggro = nullptr;
    ArcherAttackConfig* m_attackConfig = nullptr;

    Vector3 m_lastTargetPosition = Vector3::Zero;

    float m_somersaultCooldownTimer = 0.0f;
    float m_arrowBarrageCooldownTimer = 0.0f;
};
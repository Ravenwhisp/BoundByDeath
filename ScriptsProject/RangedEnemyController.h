#pragma once

#include "ScriptAPI.h"

class Transform;

class RangedEnemyController : public Script
{
    DECLARE_SCRIPT(RangedEnemyController)

public:
    explicit RangedEnemyController(GameObject* owner);

    void Start() override;
    void Update() override;

    GameObject* GetTarget() const;
    bool HasTarget() const;

    bool IsTargetDetected() const;
    bool IsTargetInAttackRange() const;
    bool IsTargetLost() const;

    float GetDistanceToTarget() const;
    bool MoveTowardsTarget();
    float GetHealth() const;
    float GetMaxHealth() const;
    bool IsAlive() const;
    void ApplyDamage(float damage);

    void Kill();
    bool IsDead() const;
    bool TrySendDeathTrigger();

    ScriptFieldList getExposedFields() const override;

private:
    GameObject* findPlayer() const;
    bool RebuildPathToTarget();
    void ClearPath();

public:
    float m_detectionRadius = 8.0f;
    float m_attackRadius = 2.5f;
    float m_loseRadius = 12.0f;
    float m_moveSpeed = 3.5f;
    float m_pathPointReachDistance = 0.25f;
    bool m_debugEnabled = true;
    float m_repathInterval = 0.5f;
    float m_turnSpeedDegrees = 360.0f;
    float m_maxHealth = 100.0f;
    float m_health = 100.0f;

private:
    GameObject* m_target = nullptr;

    std::vector<Vector3> m_path;
    size_t m_currentPathIndex = 0;
    bool m_hasPath = false;
    float m_repathTimer = 0.0f;
    Vector3 m_lastTargetPosition = Vector3::Zero;
    bool m_isDead = false;
    bool m_deathTransitionRequested = false;
};
#pragma once

#include "ScriptAPI.h"

class EnemyController : public Script
{
    DECLARE_SCRIPT(EnemyController)

public:
    explicit EnemyController(GameObject* owner);
    ~EnemyController() override = default;

    void Start() override final;
    void Update() override final;

    bool IsDead() const { return m_isDead; }
    bool HasTarget() const { return m_target != nullptr; }

    GameObject* GetTarget() const { return m_target; }
    void SetTarget(GameObject* target) { m_target = target; }

    bool IsTargetDetected() const;
    bool IsTargetInAttackRange() const;
    bool IsTargetLost() const;

    virtual void MoveToTarget() {}
    virtual void StopMoving() {}
    virtual void FaceTarget() {}
    virtual void Attack() {}

    void TakeDamage(float damage);

protected:
    virtual void OnStart() {}
    virtual void OnUpdate() {}
    virtual void OnDamaged(float damage) {}
    virtual void OnDeath() {}

    float GetDistanceToTarget() const;

protected:
    GameObject* m_target = nullptr;

    float m_health = 100.0f;
    float m_maxHealth = 100.0f;

    float m_detectionRange = 10.0f;
    float m_attackRange = 2.0f;
    float m_loseTargetRange = 15.0f;

    bool m_isDead = false;

    //ScriptFieldList getExposedFields() const override;
};
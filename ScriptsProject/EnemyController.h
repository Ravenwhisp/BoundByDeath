#pragma once

#include "ScriptAPI.h"

class EnemyDetectionAggro;

class EnemyController : public Script
{
    DECLARE_SCRIPT(EnemyController)

public:
    explicit EnemyController(GameObject* owner);

    void Start() override;
    void Update() override;
    void drawGizmo() override;

    ScriptFieldList getExposedFields() const override;

public:
    float m_combatRange = 2.0f;
    float m_moveSpeed = 1.0f;
    float m_turnSpeed = 2.0f;
    float m_intervalRepath = 0.4f;
    float m_hitReactionCooldown = 0.35f;
    bool m_debugEnabled = true;
    float m_attackEnterRangeBonus = 0.45f;
    float m_attackExitRangeBonus = 0.80f;


    bool isTargetInAttackEnterRange() const;
    bool isTargetInAttackExitRange() const;

private:
    EnemyDetectionAggro* m_enemyDetectionAggro = nullptr;
    Transform* m_currentTarget = nullptr;
    float m_repathTimer = 0.0f;
    float m_chargeCooldownTimer = 0.0f;
    std::vector<Vector3> m_path;
    bool m_hasPath = false;
    size_t m_currentIndex = 0;
    size_t m_maxPathPoints = 32;
    Vector3 m_searchExtents = Vector3(5.0f, 5.0f, 5.0f);
    float m_hitReactionCooldownTimer = 0.0f;
    bool m_pendingDamageTaken = false;
    bool m_pendingStun = false;
    const float RADIANS_TO_DEGREES = 180.0f / 3.14159265f;

public:
    bool hasValidTarget() const;
    void updateCurrentTarget();
    bool isTargetInCombatRange() const;
    Transform* getCurrentTarget() const { return m_currentTarget; }
    void clearPath();
    bool buildPathToTarget();
    void followPath();
    void faceCurrentTarget();
    void resetRepathTimer();
    void addToRepathTimer(float dt);
    bool shouldRepath() const;
    void tickChargeCooldown(float dt);
    bool isChargeReady() const;
    void consumeChargeCooldown(float cooldownDuration);
    void tickHitReactionCooldown(float dt);
    bool canTriggerHitReaction() const;
    void startHitReactionCooldown();

    void requestDamageTaken();
    void requestStun();

    bool hasPendingDamageTaken() const;
    bool hasPendingStun() const;

    void consumeDamageTaken();
    void consumeStun();

    bool tryInterruptWithReactiveState(AnimationComponent* animation);

private:
    Vector3 getChasePosition() const;
    void rotateTowardsDirection(const Vector3& direction);
};
#pragma once

#include "ScriptAPI.h"
#include <vector>

class ArrowPool;
class PlayerState;
class PlayerRotation;
class PlayerAnimationController;

class LyrielChargedAttack : public Script
{
    DECLARE_SCRIPT(LyrielChargedAttack)

public:
    explicit LyrielChargedAttack(GameObject* owner);

    void Start() override;
    void Update() override;
    void drawGizmo() override;

    ScriptFieldList getExposedFields() const override;

private:
    void updateCooldown();
    void updateAttackStateTimer();

    void beginCharge();
    void updateCharge();
    void releaseChargeAndShoot();

    bool canStartCharge() const;
    bool canShoot() const;

    Transform* findArrowSpawnTransform() const;

    Vector3 computeAimDirection() const;
    void faceDirection(const Vector3& direction);
    Vector3 getFallbackFacingDirection() const;

    float computeChargedDamage() const;

    void collectEnemiesInLine(const Vector3& origin, const Vector3& forward, std::vector<GameObject*>& outTargets);
    void applyChargedDamage(const std::vector<GameObject*>& targets, float damage);

    void spawnChargedArrow(const Vector3& origin, const Vector3& forward, float chargeRatio);
    void drawChargePreview(const Vector3& origin, const Vector3& forward, float chargeRatio) const;

    bool isAimStickValid(const Vector3& direction) const;

public:
    int m_playerIndex = 0;

    float m_minDamage = 5.0f;
    float m_maxDamage = 30.0f;
    float m_maxChargeTime = 2.0f;

    float m_attackRange = 10.0f;
    float m_lineHalfWidth = 0.75f;

    float m_attackCooldown = 4.0f;
    float m_attackLockDuration = 0.3f;

    float m_arrowSpeed = 20.0f;

    std::string m_arrowSpawnChildName = "ArrowSpawn";

private:
    ArrowPool* m_arrowPool = nullptr;
    PlayerState* m_playerState = nullptr;
    PlayerRotation* m_playerRotation = nullptr;
    PlayerAnimationController* m_playerAnimationController = nullptr;

    float m_cooldownTimer = 0.0f;
    float m_attackStateTimer = 0.0f;

    bool m_isCharging = false;
    float m_chargeTimer = 0.0f;

    Vector3 m_currentAimDirection = Vector3::Zero;
    Vector3 m_attackFacingDirection = Vector3::Zero;
};
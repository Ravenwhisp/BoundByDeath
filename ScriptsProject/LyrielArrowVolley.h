#pragma once

#include "ScriptAPI.h"
#include <vector>

class ArrowPool;
class PlayerState;
class PlayerRotation;
class PlayerAnimationController;

class LyrielArrowVolley : public Script
{
    DECLARE_SCRIPT(LyrielArrowVolley)

public:
    explicit LyrielArrowVolley(GameObject* owner);

    void Start() override;
    void Update() override;
    void drawGizmo() override;

    ScriptFieldList getExposedFields() const override;

private:
    void updateCooldown();
    void updateAttackStateTimer();

    void beginAim();
    void updateAim();
    void releaseAimAndCast();

    bool canStartAim() const;
    bool canCast() const;

    Transform* findArrowSpawnTransform() const;

    Vector3 computeAimDirection() const;
    void faceDirection(const Vector3& direction);

    void collectEnemiesInCone(const Vector3& origin, const Vector3& forward, std::vector<GameObject*>& outTargets);
    void applyVolleyDamage(const std::vector<GameObject*>& targets);
    void spawnVolleyArrows(const Vector3& origin, const Vector3& forward);

    void drawAimPreview(const Vector3& origin, const Vector3& forward) const;

    bool isAimStickValid(const Vector3& direction) const;

public:
    int m_playerIndex = 0;

    float m_volleyDamage = 20.0f;
    float m_volleyCooldown = 5.0f;
    float m_volleyRange = 8.0f;
    float m_coneAngleDegrees = 50.0f;

    int m_numVisualArrows = 5;
    float m_arrowSpeed = 18.0f;

    float m_attackLockDuration = 0.2f;

    std::string m_arrowSpawnChildName = "ArrowSpawn";

private:
    ArrowPool* m_arrowPool = nullptr;
    PlayerState* m_playerState = nullptr;
    PlayerRotation* m_playerRotation = nullptr;
    PlayerAnimationController* m_playerAnimationController = nullptr;

    float m_cooldownTimer = 0.0f;
    float m_attackStateTimer = 0.0f;

    Vector3 m_attackFacingDirection = Vector3::Zero;

    bool m_isAiming = false;
    Vector3 m_currentAimDirection = Vector3::Zero;
};
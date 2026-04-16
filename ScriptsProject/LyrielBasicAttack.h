#pragma once

#include "ScriptAPI.h"

class PlayerTargetController;
class ArrowPool;
class LyrielArrowProjectile;
class PlayerState;
class PlayerRotation;
class PlayerAnimationController;

class LyrielBasicAttack : public Script
{
    DECLARE_SCRIPT(LyrielBasicAttack)

public:
    explicit LyrielBasicAttack(GameObject* owner);

    void Start() override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

private:
    void tryAttack();
    bool spawnArrowToTarget(GameObject* target);
    Transform* findArrowSpawnTransform() const;
    void faceTarget(GameObject* target);
    void updateCooldown();
    void updateAttackStateTimer();

    void setAbilityLocked(bool locked);

private:
    PlayerTargetController* m_targetController = nullptr;
    ArrowPool* m_arrowPool = nullptr;
    PlayerState* m_playerState = nullptr;
    PlayerRotation* m_playerRotation = nullptr;
    PlayerAnimationController* m_playerAnimationController = nullptr;

    float m_cooldownTimer = 0.0f;
    float m_attackStateTimer = 0.0f;

    GameObject* m_attackFacingTarget = nullptr;

public:
    int m_playerIndex = 0;
    float m_attackDamage = 10.0f;
    float m_attackCooldown = 0.4f;
    float m_arrowSpeed = 18.0f;
    float m_attackLockDuration = 0.2f;
    std::string m_arrowSpawnChildName = "ArrowSpawn";
};
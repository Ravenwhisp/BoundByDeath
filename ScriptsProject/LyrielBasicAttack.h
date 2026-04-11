#pragma once

#include "ScriptAPI.h"

class PlayerTargetController;
class ArrowPool;
class LyrielArrowProjectile;

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
    void spawnArrowToTarget(GameObject* target);
    Transform* findArrowSpawnTransform() const;

private:
    PlayerTargetController* m_targetController = nullptr;
    ArrowPool* m_arrowPool = nullptr;

    float m_cooldownTimer = 0.0f;

public:
    int m_playerIndex = 0;
    float m_attackDamage = 10.0f;
    float m_attackCooldown = 0.4f;
    float m_arrowSpeed = 18.0f;
    std::string m_arrowSpawnChildName = "ArrowSpawn";
};
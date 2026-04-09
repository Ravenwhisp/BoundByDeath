#pragma once

#include "ScriptAPI.h"

class EnemyDetectionAggro;

class EnemyNavigation : public Script
{
    DECLARE_SCRIPT(EnemyNavigation)

private:
    enum class NavigationState
    {
        Idle,
        Chase
    };

public:
    explicit EnemyNavigation(GameObject* owner);

    void Start() override;
    void Update() override;
    void drawGizmo() override;

    ScriptFieldList getExposedFields() const override;

public:
    float m_combatRange = 2.0f;
    float m_moveSpeed = 1.0f;
    float m_turnSpeed = 2.0f;
    float m_intervalRepath = 0.4f;
    bool m_debugEnabled = true;
    std::string m_idleAnimationTrigger = "";
    std::string m_chaseAnimationTrigger = "";

private:
    EnemyDetectionAggro* m_enemyDetectionAggro = nullptr;
    NavigationState m_currentState = NavigationState::Idle;
    Transform* m_currentTarget = nullptr;
    float m_repathTimer = 0.0f;

    std::vector<Vector3> m_path;
    bool m_hasPath = false;
    size_t m_currentIndex = 0;
    size_t m_maxPathPoints = 32;
    Vector3 m_searchExtents = Vector3(5.0f, 5.0f, 5.0f);
    const float RADIANS_TO_DEGREES = 180.0f / 3.14159265f;

    AnimationComponent* m_animation = nullptr;

private:
    bool hasValidTarget() const;
    bool isTargetInCombatRange() const;

    void enterIdleState();
    void enterChaseState();

    void clearPath();
    bool buildPathToTarget();
    void followPath();
    Vector3 getChasePosition() const;

    void updateIdle();
    void updateChase();
    void rotateTowardsDirection(const Vector3& direction);
};
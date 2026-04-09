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
    float m_turnSpeed = 1.0f;
    float m_intervalRepath = 0.4f;
    bool m_debugEnabled = true;

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


private:
    bool hasValidTarget() const;
    bool isTargetInCombatRange() const;

    void clearPath();
    bool buildPathToTarget();
    void followPath();
    Vector3 getChasePosition() const;

    void updateIdle();
    void updateChase();
};
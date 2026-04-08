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
    //void drawGizmo() override;

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
};
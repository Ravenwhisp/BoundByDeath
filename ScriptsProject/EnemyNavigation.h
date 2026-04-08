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
    ScriptComponentRef<EnemyDetectionAggro> m_enemyDetectionAggro;

};
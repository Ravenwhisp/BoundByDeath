#pragma once

#include "ScriptAPI.h"

class Transform;
class PlayerTargetController;

class TargetIndicatorUI : public Script
{
    DECLARE_SCRIPT(TargetIndicatorUI)

public:
    explicit TargetIndicatorUI(GameObject* owner);

    void Start() override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

private:
    PlayerTargetController* getPlayerTargetController() const;
    void hideIndicator();
    void showIndicator();

    void startSwitchAnimation();
    void updateSwitchAnimation(Transform* visualTransform);

public:
    ScriptComponentRef<Transform> m_playerTransform;
    ScriptComponentRef<Transform> m_indicatorVisualTransform;

    Vector3 m_positionOffset = Vector3(0.0f, 0.05f, 0.0f);
    float m_followSharpness = 20.0f;

    float m_switchPopScale = 1.25f;
    float m_switchPopDuration = 0.18f;

private:
    PlayerTargetController* m_playerTargetController = nullptr;
    GameObject* m_previousTarget = nullptr;

    Vector3 m_baseScale = Vector3(1.0f, 1.0f, 1.0f);
    float m_switchAnimationTimer = 0.0f;
};
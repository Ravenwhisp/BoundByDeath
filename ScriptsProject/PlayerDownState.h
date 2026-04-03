#pragma once

#include "ScriptAPI.h"
#include "ScriptComponentRef.h"

class Damageable;
class Transform;

class PlayerDownState : public Script
{
    DECLARE_SCRIPT(PlayerDownState)

public:
    explicit PlayerDownState(GameObject* owner);

    void Start() override;
    void Update() override;
    ScriptFieldList getExposedFields() const override;

    void drawGizmo() override;

    void enterDownState();

    bool isDowned() const { return m_isDowned; }
    float getReviveProgress() const;

private:
    Damageable* findDamageable() const;
    bool isTeammateInAssistRange() const;
    void completeRevive();

    void disableGameplay();
    void enableGameplay();

public:
    float m_selfReviveTime = 10.0f;
    float m_assistRadius = 3.0f;
    float m_assistSpeedMultiplier = 2.0f;
    float m_reviveHp = 50.0f;

    ScriptComponentRef<Transform> m_teammateTransform;

private:
    Damageable* m_damageable = nullptr;

    bool m_isDowned = false;
    float m_reviveProgress = 0.0f;
};
#pragma once

#include "ScriptAPI.h"

class PlayerTargetController;

class TargetAttackController : public Script
{
    DECLARE_SCRIPT(TargetAttackController)

public:
    explicit TargetAttackController(GameObject* owner);

    void Start() override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

public:
    float m_attackDamage = 10.0f;

private:
    PlayerTargetController* m_targetController = nullptr;
    bool m_attackWasDown = false;

private:
    PlayerTargetController* getTargetController() const;
    bool isAttackJustPressed();
    void attackCurrentTarget();
};
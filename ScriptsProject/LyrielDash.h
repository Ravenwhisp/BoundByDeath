#pragma once

#include "AbilityDash.h"
#include "Transform2D.h"

class LyrielDash : public AbilityDash
{
    DECLARE_SCRIPT(LyrielDash)

public:
    explicit LyrielDash(GameObject* owner);

    void Start() override;
    ScriptFieldList getExposedFields() const override;

    void recoverCharge();

protected:
    bool canDash() const override;
    void onDashStarted() override;
    void onDashUpdate(float dt) override;
    void updateUI() override;
    void updateChargeVisual(Transform2D* transform, float& currentScale, float targetScale, float dt);

private:
    // Will be implemented to MathAPI
    float moveTowards(float current, float target, float maxDelta);

private:

    float m_chargeRechargeTime = 3.0f;
    int m_maxCharges = 3;
    int m_currentCharges = 0;
    float m_chargeRecoveryTimer = 0.0f;

    ScriptComponentRef<Transform2D> m_charge1UI;
    ScriptComponentRef<Transform2D> m_charge2UI;
    ScriptComponentRef<Transform2D> m_charge3UI;

    Transform2D* m_charge1Transform2D = nullptr;
    Transform2D* m_charge2Transform2D = nullptr;
    Transform2D* m_charge3Transform2D = nullptr;

private:

    float chargedScale = 1.0f;
    float emptyScale = 0.5f;
    float uiScaleSpeed = 3.0f;

    float m_charge1Scale = chargedScale;
    float m_charge2Scale = chargedScale;
    float m_charge3Scale = chargedScale;
};
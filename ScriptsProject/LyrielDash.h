#pragma once

#include "AbilityDash.h"

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
    bool validateDashTarget() override;
    void drawGizmo() override;


private:
    float m_chargeRechargeTime = 3.0f;
    int m_maxCharges = 3;
    int m_currentCharges = 0;
    float m_chargeRecoveryTimer = 0.0f;

private:
    // For debugging only
    Vector3 m_debugDashStart = Vector3::Zero;
    Vector3 m_debugDashCandidateEnd = Vector3::Zero;
    Vector3 m_debugDashSampleEnd = Vector3::Zero;
    bool m_debugLastDashValid = false;
};
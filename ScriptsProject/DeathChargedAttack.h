#pragma once

#include "DeathAbilityBase.h"

class DeathChargedAttack : public DeathAbilityBase
{
    DECLARE_SCRIPT(DeathChargedAttack)

public:
    explicit DeathChargedAttack(GameObject* owner);

    void Start()     override;
    void Update()    override;
    void drawGizmo() override;

    ScriptFieldList getExposedFields() const override;

    ScriptComponentRef<Transform> m_ChargedAttackUI;

public:
    float m_minChargeTime         = 0.5f;
    float m_attackLockDuration    = 0.4f;
    float m_finalHitLockDuration  = 0.8f;
    float m_chargedArcRange       = 3.5f;
    float m_chargedArcAngle       = 150.0f;

protected:
    void onAttackWindowUpdate()   override;
    void onAttackWindowFinished() override;

private:
    void startCharging();
    void fireAttack();
    void updateAimDirection();
    void snapFaceAimDirection();
    void releaseComboMoveLock();

    float   m_chargeTime             = 0.0f;
    bool    m_isCharging             = false;
    bool    m_movementLockedForCombo = false;
    Vector3 m_aimDirection           = { 0.0f, 0.0f, 0.0f };
};

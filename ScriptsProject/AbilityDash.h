#pragma once

#include "AbilityBase.h"

#define MAX_DASH_CHARGES 3

class PlayerController;
class PlayerMovement;

class AbilityDash : public AbilityBase
{
    DECLARE_SCRIPT(AbilityDash)

public:
    explicit AbilityDash(GameObject* owner);

    void Start() override;
    void Update() override;
    void drawGizmo() override;

    void recoverCharge();

    ScriptFieldList getExposedFields() const override;

private:
    void tryStartDash();
    void updateDash(float dt);
    void stopDash();

    void updateChargeRecovery(float dt);
    void calculateDashMovement(float dt);

    PlayerController* findControllerScript(GameObject* owner) const;
    PlayerMovement* findMovementScript(GameObject* owner) const;

public:
    float m_dashCooldown = 0.5f;
    float m_dashDuration = 0.15f;
    float m_dashDistance = 3.0f;
    float m_chargeRechargeTime = 3.0f;

    bool m_debugEnabled = false;

private:
    PlayerController* m_playerController = nullptr;
    PlayerMovement* m_playerMovement = nullptr;

    float m_dashTimer = 0.0f;
    float m_chargeRecoveryTimer = 0.0f;

    int m_charges = MAX_DASH_CHARGES;
    bool m_isDashing = false;

    Vector3 m_dashDirection = Vector3::Zero;
};
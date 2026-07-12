#pragma once

#include "AbilityBase.h"

class PlayerMovement;

class ChargedAttackBase : public AbilityBase
{
public:
    explicit ChargedAttackBase(GameObject* owner);

    void Start() override;

    ScriptFieldList getExposedFields() const override;

protected:
    void applyChargingMovementSlowdown();
    void resetChargingMovementSlowdown();

private:
    PlayerMovement* m_playerMovement = nullptr;

protected:
    float m_chargeMovementSlowdownPercentage = 90.0f;
};
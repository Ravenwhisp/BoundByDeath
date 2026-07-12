#pragma once

#include "AbilityBase.h"

class PlayerMovement;

class ChargedAttackBase : public AbilityBase
{
public:
    explicit ChargedAttackBase(GameObject* owner);

    void Start() override;

protected:
    void applyChargingMovementSlowdown(float slowdownPercentage);
    void resetChargingMovementSlowdown();

private:
    PlayerMovement* m_playerMovement = nullptr;
};
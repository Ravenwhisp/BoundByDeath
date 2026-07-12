#include "pch.h"
#include "ChargedAttackBase.h"

#include "PlayerMovement.h"

IMPLEMENT_SCRIPT_FIELDS_INHERITED(ChargedAttackBase, AbilityBase,
    SERIALIZED_FLOAT(m_chargeMovementSlowdownPercentage, "Charge Movement Slowdown (%)", 0.0f, 100.0f, 1.0f)
)

ChargedAttackBase::ChargedAttackBase(GameObject* owner)
    : AbilityBase(owner)
{
}

void ChargedAttackBase::Start()
{
    AbilityBase::Start();

    m_playerMovement = GameObjectAPI::findScript<PlayerMovement>(getOwner());

    if (m_playerMovement == nullptr)
    {
        Debug::warn("[ChargedAttackBase] PlayerMovement not found on owner '%s'.", GameObjectAPI::getName(getOwner()));
    }
}

void ChargedAttackBase::applyChargingMovementSlowdown()
{
    if (m_playerMovement == nullptr)
    {
        return;
    }

    const float movementMultiplier = 1.0f - m_chargeMovementSlowdownPercentage / 100.0f;
    m_playerMovement->setMovementMultiplier(movementMultiplier);
}

void ChargedAttackBase::resetChargingMovementSlowdown()
{
    if (m_playerMovement == nullptr)
    {
        return;
    }

    m_playerMovement->resetMovementMultiplier();
}


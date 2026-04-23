#include "pch.h"
#include "LyrielDash.h"

IMPLEMENT_SCRIPT_FIELDS(LyrielDash,
    SERIALIZED_FLOAT(m_dashDurationLyriel, "Dash Duration", 0.0f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_dashDistanceLyriel, "Dash Distance", 0.0f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_dashCooldown, "Dash Cooldown", 0.0f, 5.0f, 0.01f),
    SERIALIZED_FLOAT(m_chargeRechargeTime, "Charge Recharge Time", 0.1f, 10.0f, 0.1f)
)

LyrielDash::LyrielDash(GameObject* owner)
    : AbilityDash(owner)
{
}

void LyrielDash::Start()
{
    m_dashDuration = m_dashDurationLyriel;
    m_dashDistance = m_dashDistanceLyriel;
    m_cooldown = m_dashCooldown;

    AbilityDash::Start();
}

void LyrielDash::recoverCharge()
{
    if (m_charges < MAX_DASH_CHARGES)
    {
        ++m_charges;

        if (m_charges == MAX_DASH_CHARGES)
        {
            m_chargeRecoveryTimer = 0.0f;
        }
    }
}

bool LyrielDash::canDash() const
{
    return m_charges > 0;
}

void LyrielDash::onDashStarted()
{
    --m_charges;
}

void LyrielDash::onDashUpdate(float dt)
{
    if (m_charges >= MAX_DASH_CHARGES)
    {
        return;
    }

    m_chargeRecoveryTimer += dt;

    while (m_chargeRecoveryTimer >= m_chargeRechargeTime && m_charges < MAX_DASH_CHARGES)
    {
        ++m_charges;
        m_chargeRecoveryTimer -= m_chargeRechargeTime;
    }

    if (m_charges >= MAX_DASH_CHARGES)
    {
        m_charges = MAX_DASH_CHARGES;
        m_chargeRecoveryTimer = 0.0f;
    }
}

IMPLEMENT_SCRIPT(LyrielDash)
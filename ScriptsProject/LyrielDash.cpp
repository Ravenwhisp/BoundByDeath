#include "pch.h"
#include "LyrielDash.h"

static const ScriptFieldInfo LyrielDashFields[] =
{
    { "Charge Recharge Time", ScriptFieldType::Float, offsetof(LyrielDash, m_chargeRechargeTime), { 0.1f, 10.0f, 0.1f } }
};

IMPLEMENT_SCRIPT_FIELDS(LyrielDash, LyrielDashFields)

LyrielDash::LyrielDash(GameObject* owner)
    : AbilityDash(owner)
{
}

void LyrielDash::Start()
{
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
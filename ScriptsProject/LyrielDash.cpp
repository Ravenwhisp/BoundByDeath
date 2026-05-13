#include "pch.h"
#include "LyrielDash.h"

#include "LyrielCharacter.h"

IMPLEMENT_SCRIPT_FIELDS_INHERITED(LyrielDash, AbilityDash,
    SERIALIZED_FLOAT(m_chargeRechargeTime, "Charge Recharge Time", 0.1f, 10.0f, 0.1f),
    SERIALIZED_INT(m_maxCharges, "Max charges")
)

LyrielDash::LyrielDash(GameObject* owner)
    : AbilityDash(owner)
{
}

void LyrielDash::Start()
{
    AbilityDash::Start();

    if (m_character == nullptr)
    {
        Debug::log("[LyrielDash] CharacterBase not found on owner '%s'.", GameObjectAPI::getName(getOwner()));
    }

    m_currentCharges = m_maxCharges;
}

void LyrielDash::recoverCharge()
{
    if (m_currentCharges < m_maxCharges)
    {
        ++m_currentCharges;

        if (m_currentCharges == m_maxCharges)
        {
            m_chargeRecoveryTimer = 0.0f;
        }
    }
}

bool LyrielDash::canDash() const
{
    return m_currentCharges > 0;
}

void LyrielDash::onDashStarted()
{
    --m_currentCharges;
}

void LyrielDash::onDashUpdate(float dt)
{
    if (m_currentCharges >= m_maxCharges)
    {
        return;
    }

    m_chargeRecoveryTimer += dt;

    while (m_chargeRecoveryTimer >= m_chargeRechargeTime && m_currentCharges < m_maxCharges)
    {
        ++m_currentCharges;
        m_chargeRecoveryTimer -= m_chargeRechargeTime;
    }

    if (m_currentCharges >= m_maxCharges)
    {
        m_currentCharges = m_maxCharges;
        m_chargeRecoveryTimer = 0.0f;
    }
}

bool LyrielDash::validateDashTarget()
{
    Vector3 currentPosition = TransformAPI::getPosition(getOwner()->GetTransform());

    Vector3 candidateEnd = currentPosition + m_dashDirection * m_dashDistance;

    Vector3 sampledPosition;
    Vector3 searchExtents = Vector3(1.0f, 2.0f, 1.0f);

    if (NavigationAPI::samplePosition(candidateEnd, sampledPosition, searchExtents, NavAgentProfile::PlayerNormal))
    {
        m_dashTargetPosition = sampledPosition;
        m_hasDashTarget = true;
        Debug::log("Dash target set: %s | Position: %.2f %.2f %.2f",
            m_hasDashTarget ? "true" : "false",
            m_dashTargetPosition.x,
            m_dashTargetPosition.y,
            m_dashTargetPosition.z);
        return true;
    }

    return false;
}

IMPLEMENT_SCRIPT(LyrielDash)
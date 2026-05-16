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
    m_debugDashStart = currentPosition; // Debugging

    Vector3 candidateEnd = currentPosition + m_dashDirection * m_dashDistance;
    m_debugDashCandidateEnd = candidateEnd; // Debugging

    Vector3 sampledPosition;
    Vector3 searchExtents = Vector3(1.0f, 2.0f, 1.0f);

    if (NavigationAPI::samplePosition(candidateEnd, sampledPosition, searchExtents, NavAgentProfile::PlayerNormal))
    {
        m_dashTargetPosition = sampledPosition;
        m_hasDashTarget = true;
        m_debugDashSampleEnd = sampledPosition; // Debugging
        m_debugLastDashValid = true; // Debugging

        return true;
    }

    m_debugLastDashValid = false; // Debugging
    return false;
}

void LyrielDash::drawGizmo()
{
    const Vector3 white = { 1.0f, 1.0f, 1.0f };
    const Vector3 yellow = { 1.0f, 1.0f, 0.0f };
    const Vector3 cyan = { 0.0f, 1.0f, 1.0f };
    const Vector3 red = { 1.0f, 0.0f, 0.0f };
    const Vector3 up = { 0.0f, 1.0f, 0.0f };

    DebugDrawAPI::drawArrow(m_debugDashStart, m_debugDashCandidateEnd, white, 0.25f);
    DebugDrawAPI::drawCircle(m_debugDashCandidateEnd, up, yellow, 0.45f);

    if (m_debugLastDashValid)
    {
        DebugDrawAPI::drawCircle(m_debugDashSampleEnd, up, cyan, 0.35f);
    }
    else
    {
        DebugDrawAPI::drawCircle(m_debugDashCandidateEnd, up, red, 0.55f);
    }
}

IMPLEMENT_SCRIPT(LyrielDash)
#include "pch.h"
#include "PlayerDownState.h"
#include "Damageable.h"

static const ScriptFieldInfo playerDownStateFields[] =
{
    { "Self Revive Time", ScriptFieldType::Float, offsetof(PlayerDownState, m_selfReviveTime), { 0.1f, 999999.0f, 0.1f } },
    { "Assist Radius", ScriptFieldType::Float, offsetof(PlayerDownState, m_assistRadius), { 0.0f, 999999.0f, 0.1f } },
    { "Assist Speed Multiplier", ScriptFieldType::Float, offsetof(PlayerDownState, m_assistSpeedMultiplier), { 1.0f, 999999.0f, 0.1f } },
    { "Revive HP", ScriptFieldType::Float, offsetof(PlayerDownState, m_reviveHp), { 1.0f, 999999.0f, 1.0f } },
    { "Teammate Transform", ScriptFieldType::ComponentRef, offsetof(PlayerDownState, m_teammateTransform), {}, {}, { ComponentType::TRANSFORM } }
};

IMPLEMENT_SCRIPT_FIELDS(PlayerDownState, playerDownStateFields)

PlayerDownState::PlayerDownState(GameObject* owner)
    : Script(owner)
{
}

void PlayerDownState::Start()
{
    m_damageable = findDamageable();

    if (!m_damageable)
    {
        Debug::warn("PlayerDownState on '%s' could not find a Damageable on the same GameObject.", GameObjectAPI::getName(m_owner));
    }
}

void PlayerDownState::Update()
{
    if (!m_isDowned)
    {
        return;
    }

    if (!m_damageable)
    {
        return;
    }

    float speedMultiplier = 1.0f;
    if (isTeammateInAssistRange()) 
    {
        speedMultiplier *= m_assistSpeedMultiplier;
    }

    m_reviveProgress += Time::getDeltaTime() * speedMultiplier;

    if (m_reviveProgress >= m_selfReviveTime)
    {
        completeRevive();
    }
}

void PlayerDownState::drawGizmo()
{
    Transform* transform = GameObjectAPI::getTransform(m_owner);
    Vector3 position = TransformAPI::getPosition(transform);

    Vector3 circleColor = Vector3(0.0f, 1.0f, 0.0f);
    if (m_isDowned)
    {
        circleColor = Vector3(1.0f, 1.0f, 0.0f);
    }

    DebugDrawAPI::drawCircle(position, Vector3(0.0f, 1.0f, 0.0f), circleColor, m_assistRadius, 32.0f, 0, true);
}

void PlayerDownState::enterDownState()
{
    if (m_isDowned)
    {
        return;
    }

    m_isDowned = true;
    m_reviveProgress = 0.0f;

    if (m_damageable)
    {
        m_damageable->setInvulnerable(true);
    }

    disableGameplay();

    Debug::log("%s entered down state.", GameObjectAPI::getName(m_owner));
}

float PlayerDownState::getReviveProgress() const
{
    if (m_selfReviveTime <= 0.0f)
    {
        return 1.0f;
    }

    float progress = m_reviveProgress/m_selfReviveTime;

    if (progress < 0.0f)
    {
        return 0.0f;
    }

    if (progress > 1.0f)
    {
        return 1.0f;
    }

    return progress;
}

Damageable* PlayerDownState::findDamageable() const
{
    Script* script = GameObjectAPI::getScript(m_owner, "PlayerDamageable");
    Damageable* damageable = dynamic_cast<Damageable*>(script);

    if (damageable)
    {
        return damageable;
    }

    script = GameObjectAPI::getScript(m_owner, "Damageable");
    damageable = dynamic_cast<Damageable*>(script);

    return damageable;
}

bool PlayerDownState::isTeammateInAssistRange() const
{
    Transform* ownTransform = GameObjectAPI::getTransform(m_owner);
    Transform* teammateTransform = m_teammateTransform.getReferencedComponent();

    if (!ownTransform || !teammateTransform)
    {
        return false;
    }

    Vector3 ownPosition = TransformAPI::getPosition(ownTransform);
    Vector3 teammatePosition = TransformAPI::getPosition(teammateTransform);

    float distance = (teammatePosition - ownPosition).Length();

    return distance <= m_assistRadius;
}

void PlayerDownState::completeRevive()
{
    m_isDowned = false;
    m_reviveProgress = 0.0f;

    if (m_damageable)
    {
        m_damageable->setInvulnerable(false);
        m_damageable->revive(m_reviveHp);
    }

    enableGameplay();

    Debug::log("%s completed revive from down state.", GameObjectAPI::getName(m_owner));
}

void PlayerDownState::disableGameplay()
{
    Debug::log("%s gameplay disabled due to down state. Need implementation!", GameObjectAPI::getName(m_owner));
}

void PlayerDownState::enableGameplay()
{
    Debug::log("%s gameplay re-enabled after revive. Need implementation!", GameObjectAPI::getName(m_owner));
}

IMPLEMENT_SCRIPT(PlayerDownState)
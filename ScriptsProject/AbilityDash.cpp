#include "pch.h"
#include "AbilityDash.h"

#include "CharacterBase.h"
#include "PlayerController.h"
#include "PlayerMovement.h"

#define PI 3.1415926535897931f

static const ScriptFieldInfo AbilityDashFields[] =
{
    { "Dash Cooldown",        ScriptFieldType::Float, offsetof(AbilityDash, m_dashCooldown),       { 0.0f, 5.0f, 0.01f } },
    { "Dash Duration",        ScriptFieldType::Float, offsetof(AbilityDash, m_dashDuration),       { 0.0f, 1.0f, 0.01f } },
    { "Dash Distance",        ScriptFieldType::Float, offsetof(AbilityDash, m_dashDistance),       { 0.0f, 10.0f, 0.1f } },
    { "Charge Recharge Time", ScriptFieldType::Float, offsetof(AbilityDash, m_chargeRechargeTime), { 0.1f, 10.0f, 0.1f } },
    { "Enable Debug",         ScriptFieldType::Bool,  offsetof(AbilityDash, m_debugEnabled) }
};

IMPLEMENT_SCRIPT_FIELDS(AbilityDash, AbilityDashFields)

AbilityDash::AbilityDash(GameObject* owner)
    : AbilityBase(owner)
{
}

void AbilityDash::Start()
{
    AbilityBase::Start();

    m_cooldown = m_dashCooldown;

    m_playerController = findControllerScript(getOwner());
    m_playerMovement = findMovementScript(getOwner());

    if (m_character == nullptr)
    {
        Debug::warn("AbilityDash: CharacterBase not found on this GameObject.");
    }
}

void AbilityDash::Update()
{
    AbilityBase::Update();

    if (m_character == nullptr)
    {
        return;
    }

    const float dt = Time::getDeltaTime();

    updateChargeRecovery(dt);

    if (m_isDashing)
    {
        updateDash(dt);
        return;
    }

    if (Input::isLeftShoulderJustPressed(getPlayerIndex()))
    {
        tryStartDash();
    }
}

void AbilityDash::drawGizmo()
{
}

void AbilityDash::recoverCharge()
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

void AbilityDash::tryStartDash()
{
    if (!canStartAbility())
    {
        return;
    }

    if (m_charges <= 0)
    {
        return;
    }

    if (m_playerController == nullptr || m_playerMovement == nullptr)
    {
        return;
    }

    --m_charges;
    m_dashTimer = 0.0f;
    m_isDashing = true;

    setAbilityLocked(true);
    m_cooldownTimer = m_cooldown;

    Vector3 moveDirection = m_playerController->getMoveDirection();

    if (moveDirection.LengthSquared() <= 0.0001f)
    {
        Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
        if (ownerTransform != nullptr)
        {
            m_dashDirection = TransformAPI::getForward(ownerTransform);
        }
        else
        {
            m_dashDirection = Vector3::Zero;
        }
    }
    else
    {
        m_dashDirection = moveDirection;
    }

    m_dashDirection.y = 0.0f;
    if (m_dashDirection.LengthSquared() > 0.0001f)
    {
        m_dashDirection.Normalize();
    }
}

void AbilityDash::updateDash(float dt)
{
    m_dashTimer += dt;
    calculateDashMovement(dt);

    if (m_dashTimer >= m_dashDuration)
    {
        stopDash();
    }
}

void AbilityDash::stopDash()
{
    m_isDashing = false;
    m_dashTimer = 0.0f;
    m_dashDirection = Vector3::Zero;

    setAbilityLocked(false);
}

void AbilityDash::updateChargeRecovery(float dt)
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

void AbilityDash::calculateDashMovement(float dt)
{
    if (m_dashDuration <= 0.0001f)
    {
        return;
    }

    float t = m_dashTimer / m_dashDuration;
    t = (t < 0.0f) ? 0.0f : (t > 1.0f ? 1.0f : t);

    const float curveSpeed = 0.5f * PI * cos(t * PI * 0.5f);
    const float currentSpeed = (m_dashDistance / m_dashDuration) * curveSpeed;

    const Vector3 velocity = m_dashDirection * currentSpeed;

    if (m_playerMovement != nullptr)
    {
        m_playerMovement->playerDashMovement(getOwner(), velocity * dt);
    }
}

PlayerController* AbilityDash::findControllerScript(GameObject* owner) const
{
    Script* script = GameObjectAPI::getScript(owner, "PlayerController");
    if (script == nullptr)
    {
        Debug::warn("AbilityDash: PlayerController not found on this GameObject.");
        return nullptr;
    }

    return static_cast<PlayerController*>(script);
}

PlayerMovement* AbilityDash::findMovementScript(GameObject* owner) const
{
    Script* script = GameObjectAPI::getScript(owner, "PlayerMovement");
    if (script == nullptr)
    {
        Debug::warn("AbilityDash: PlayerMovement not found on this GameObject.");
        return nullptr;
    }

    return static_cast<PlayerMovement*>(script);
}

IMPLEMENT_SCRIPT(AbilityDash)
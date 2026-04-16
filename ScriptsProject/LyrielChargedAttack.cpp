#include "pch.h"
#include "LyrielChargedAttack.h"

#include "ArrowPool.h"
#include "LyrielArrowProjectile.h"
#include "Damageable.h"
#include "PlayerState.h"
#include "PlayerRotation.h"
#include "PlayerAnimationController.h"

#include <cmath>

static const ScriptFieldInfo LyrielChargedAttackFields[] =
{
    { "Player Index", ScriptFieldType::Int, offsetof(LyrielChargedAttack, m_playerIndex) },
    { "Min Damage", ScriptFieldType::Float, offsetof(LyrielChargedAttack, m_minDamage), { 0.0f, 100.0f, 0.5f } },
    { "Max Damage", ScriptFieldType::Float, offsetof(LyrielChargedAttack, m_maxDamage), { 0.0f, 200.0f, 0.5f } },
    { "Max Charge Time", ScriptFieldType::Float, offsetof(LyrielChargedAttack, m_maxChargeTime), { 0.1f, 5.0f, 0.05f } },
    { "Attack Range", ScriptFieldType::Float, offsetof(LyrielChargedAttack, m_attackRange), { 0.0f, 50.0f, 0.1f } },
    { "Line Half Width", ScriptFieldType::Float, offsetof(LyrielChargedAttack, m_lineHalfWidth), { 0.1f, 10.0f, 0.05f } },
    { "Attack Cooldown", ScriptFieldType::Float, offsetof(LyrielChargedAttack, m_attackCooldown), { 0.0f, 10.0f, 0.05f } },
    { "Attack Lock Duration", ScriptFieldType::Float, offsetof(LyrielChargedAttack, m_attackLockDuration), { 0.0f, 2.0f, 0.01f } },
    { "Arrow Speed", ScriptFieldType::Float, offsetof(LyrielChargedAttack, m_arrowSpeed), { 0.0f, 100.0f, 0.5f } },
    { "Arrow Spawn Child Name", ScriptFieldType::String, offsetof(LyrielChargedAttack, m_arrowSpawnChildName) }
};

IMPLEMENT_SCRIPT_FIELDS(LyrielChargedAttack, LyrielChargedAttackFields)

LyrielChargedAttack::LyrielChargedAttack(GameObject* owner)
    : Script(owner)
{
}

void LyrielChargedAttack::Start()
{
    Script* arrowPoolScript = GameObjectAPI::getScript(getOwner(), "ArrowPool");
    m_arrowPool = dynamic_cast<ArrowPool*>(arrowPoolScript);

    Script* stateScript = GameObjectAPI::getScript(getOwner(), "PlayerState");
    m_playerState = dynamic_cast<PlayerState*>(stateScript);

    Script* rotationScript = GameObjectAPI::getScript(getOwner(), "PlayerRotation");
    m_playerRotation = dynamic_cast<PlayerRotation*>(rotationScript);

    Script* animationScript = GameObjectAPI::getScript(getOwner(), "PlayerAnimationController");
    m_playerAnimationController = dynamic_cast<PlayerAnimationController*>(animationScript);

    if (m_arrowPool == nullptr)
    {
        Debug::log("[LyrielChargedAttack] ArrowPool not found on owner.");
    }

    if (m_playerState == nullptr)
    {
        Debug::log("[LyrielChargedAttack] PlayerState not found on owner.");
    }

    if (m_playerRotation == nullptr)
    {
        Debug::log("[LyrielChargedAttack] PlayerRotation not found on owner.");
    }

    if (m_playerAnimationController == nullptr)
    {
        Debug::log("[LyrielChargedAttack] PlayerAnimationController not found on owner.");
    }
}

void LyrielChargedAttack::Update()
{
    updateCooldown();

    if (m_attackStateTimer > 0.0f)
    {
        if (m_attackFacingDirection.LengthSquared() > 0.0001f)
        {
            faceDirection(m_attackFacingDirection);
        }
    }

    updateAttackStateTimer();

    if (canStartCharge() && Input::isRightTriggerJustPressed(m_playerIndex))
    {
        beginCharge();
    }

    if (m_isCharging && Input::isRightTriggerPressed(m_playerIndex))
    {
        updateCharge();
    }

    if (m_isCharging && Input::isRightTriggerReleased(m_playerIndex))
    {
        releaseChargeAndShoot();
    }
}

void LyrielChargedAttack::drawGizmo()
{
    if (!m_isCharging)
    {
        return;
    }

    Vector3 previewDirection = m_currentAimDirection;
    if (previewDirection.LengthSquared() <= 0.0001f)
    {
        previewDirection = getFallbackFacingDirection();
    }

    if (previewDirection.LengthSquared() <= 0.0001f)
    {
        return;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    const Vector3 origin = TransformAPI::getGlobalPosition(ownerTransform);

    float chargeRatio = 0.0f;
    if (m_maxChargeTime > 0.0001f)
    {
        chargeRatio = m_chargeTimer / m_maxChargeTime;
    }

    if (chargeRatio > 1.0f)
    {
        chargeRatio = 1.0f;
    }

    drawChargePreview(origin, previewDirection, chargeRatio);
}

void LyrielChargedAttack::updateCooldown()
{
    if (m_cooldownTimer <= 0.0f)
    {
        return;
    }

    m_cooldownTimer -= Time::getDeltaTime();
    if (m_cooldownTimer < 0.0f)
    {
        m_cooldownTimer = 0.0f;
    }
}

void LyrielChargedAttack::updateAttackStateTimer()
{
    if (m_attackStateTimer <= 0.0f)
    {
        return;
    }

    m_attackStateTimer -= Time::getDeltaTime();
    if (m_attackStateTimer <= 0.0f)
    {
        m_attackStateTimer = 0.0f;
        m_attackFacingDirection = Vector3::Zero;

        if (m_playerState != nullptr && m_playerState->isAttacking())
        {
            m_playerState->setState(PlayerStateType::Normal);
        }
    }
}

bool LyrielChargedAttack::canStartCharge() const
{
    if (m_cooldownTimer > 0.0f)
    {
        return false;
    }

    if (m_playerState != nullptr && m_playerState->isDowned())
    {
        return false;
    }

    return true;
}

bool LyrielChargedAttack::canShoot() const
{
    if (m_playerState != nullptr && m_playerState->isDowned())
    {
        return false;
    }

    return true;
}

void LyrielChargedAttack::beginCharge()
{
    m_isCharging = true;
    m_chargeTimer = 0.0f;
    m_currentAimDirection = Vector3::Zero;

    Vector3 aimDirection = computeAimDirection();
    if (isAimStickValid(aimDirection))
    {
        m_currentAimDirection = aimDirection;
    }
}

void LyrielChargedAttack::updateCharge()
{
    m_chargeTimer += Time::getDeltaTime();
    if (m_chargeTimer > m_maxChargeTime)
    {
        m_chargeTimer = m_maxChargeTime;
    }

    Vector3 aimDirection = computeAimDirection();
    if (isAimStickValid(aimDirection))
    {
        m_currentAimDirection = aimDirection;
    }
}

IMPLEMENT_SCRIPT(LyrielChargedAttack)
#include "pch.h"
#include "DeathChargedAttack.h"
#include "CharacterBase.h"
#include "DeathCharacter.h"
#include "PlayerTargetController.h"
#include "PlayerState.h"
#include "PlayerAnimationController.h"
#include "PlayerRotation.h"

static const ScriptFieldInfo DeathChargedAttackFields[] =
{
    { "Min Charge Time",              ScriptFieldType::Float, offsetof(DeathChargedAttack, m_minChargeTime),             { 0.0f,  3.0f, 0.05f } },
    { "Max Charge Time",              ScriptFieldType::Float, offsetof(DeathChargedAttack, m_maxChargeTime),             { 0.1f,  5.0f, 0.05f } },
    { "Attack Lock Duration",         ScriptFieldType::Float, offsetof(DeathChargedAttack, m_attackLockDuration),        { 0.05f, 2.0f, 0.05f } },
    { "Charged Attack Lock Duration", ScriptFieldType::Float, offsetof(DeathChargedAttack, m_chargedAttackLockDuration), { 0.05f, 3.0f, 0.05f } },
};

IMPLEMENT_SCRIPT_FIELDS(DeathChargedAttack, DeathChargedAttackFields)

DeathChargedAttack::DeathChargedAttack(GameObject* owner)
    : AbilityBase(owner)
{
}

void DeathChargedAttack::Start()
{
    m_deathChar = static_cast<DeathCharacter*>(
        GameObjectAPI::getScript(m_owner, "DeathCharacter"));
    m_character = m_deathChar;

    m_playerState = static_cast<PlayerState*>(
        GameObjectAPI::getScript(m_owner, "PlayerState"));

    m_animController = static_cast<PlayerAnimationController*>(
        GameObjectAPI::getScript(m_owner, "PlayerAnimationController"));

    m_playerRotation = static_cast<PlayerRotation*>(
        GameObjectAPI::getScript(m_owner, "PlayerRotation"));

    if (m_maxChargeTime < m_minChargeTime)
    {
        Debug::warn("DeathChargedAttack: Max Charge Time < Min Charge Time, clamping to Min.");
        m_maxChargeTime = m_minChargeTime;
    }

    if (m_deathChar == nullptr)
    {
        Debug::warn("DeathChargedAttack: DeathCharacter not found.");
    }
    if (m_playerState == nullptr)
    {
        Debug::warn("DeathChargedAttack: PlayerState not found.");
    }
    if (m_animController == nullptr)
    {
        Debug::warn("DeathChargedAttack: PlayerAnimationController not found.");
    }
    if (m_playerRotation == nullptr)
    {
        Debug::warn("DeathChargedAttack: PlayerRotation not found.");
    }
}

void DeathChargedAttack::Update()
{
    AbilityBase::Update();

    if (m_character == nullptr)
    {
        return;
    }

    const float dt = Time::getDeltaTime();

    if (m_attackLockTimer > 0.0f)
    {
        faceTarget(m_attackFacingTarget);

        if (m_animController != nullptr)
        {
            m_animController->requestAttack();
        }

        m_attackLockTimer -= dt;
        if (m_attackLockTimer <= 0.0f)
        {
            m_attackLockTimer = 0.0f;
            m_attackFacingTarget = nullptr;
            if (m_playerState != nullptr)
            {
                m_playerState->setState(PlayerStateType::Normal);
            }
            onDeactivate();
        }
        return;
    }

    if (Input::isRightTriggerJustPressed(getPlayerIndex()))
    {
        if (m_deathChar->isInComboCooldown() || !canActivate() || !m_deathChar->canUseR2InCombo())
        {
            return;
        }

        m_chargeTime = 0.0f;
        m_quickPressOnly = (m_deathChar->getComboStep() > 0);
        onActivate();
        return;
    }

    if (!isActive())
    {
        return;
    }

    m_chargeTime += dt;

    const bool autoFire = !m_quickPressOnly && (m_chargeTime >= m_maxChargeTime);
    const bool released = Input::isRightTriggerReleased(getPlayerIndex());

    if (!autoFire && !released)
    {
        return;
    }

    const bool chargeComplete = !m_quickPressOnly && (m_chargeTime >= m_minChargeTime);

    const int stepBefore = m_deathChar->getComboStep();
    float lockTime = m_attackLockDuration;
    float damage = m_deathChar->m_chargedAttackDamage;

    if (chargeComplete)
    {
        const float ratio = m_chargeTime / m_maxChargeTime;
        const float clampedRatio = ratio > 1.0f ? 1.0f : ratio;
        damage = m_deathChar->m_chargedAttackDamage * (1.0f + clampedRatio);
        lockTime = m_chargedAttackLockDuration;
        Debug::log("[R2] charged step %d/3  ratio=%.0f%%  dmg=%.1f", stepBefore + 1, clampedRatio * 100.0f, damage);
    }
    else
    {
        Debug::log("[R2] quick step %d/3  dmg=%.1f", stepBefore + 1, damage);
    }

    m_deathChar->dealDamageInArc(damage);
    // TODO: apply stun to hit enemies (charged only)
    m_deathChar->advanceCombo(true, chargeComplete);

    GameObject* target = m_character->getTargetController()
        ? m_character->getTargetController()->getCurrentTarget()
        : nullptr;

    faceTarget(target);
    m_attackFacingTarget = target;
    m_chargeTime = 0.0f;
    m_quickPressOnly = false;
    m_attackLockTimer = lockTime;

    if (m_animController != nullptr)
    {
        m_animController->requestAttack();
    }

    if (m_playerState != nullptr)
    {
        m_playerState->setState(PlayerStateType::Attacking);
    }
}

void DeathChargedAttack::faceTarget(GameObject* target)
{
    if (m_playerRotation == nullptr || target == nullptr)
    {
        return;
    }

    Transform* myTransform = GameObjectAPI::getTransform(m_owner);
    Transform* targetTransform = GameObjectAPI::getTransform(target);

    Vector3 myPos = TransformAPI::getGlobalPosition(myTransform);
    Vector3 targetPos = TransformAPI::getGlobalPosition(targetTransform);

    Vector3 dir = targetPos - myPos;
    dir.y = 0.0f;
    if (dir.LengthSquared() <= 0.0001f)
    {
        return;
    }
    dir.Normalize();

    m_playerRotation->applyFacingFromDirection(m_owner, dir, Time::getDeltaTime());
}

IMPLEMENT_SCRIPT(DeathChargedAttack)

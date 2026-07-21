#include "pch.h"
#include "AbilityBase.h"

#include "CharacterBase.h"
#include "PlayerState.h"
#include "PlayerAnimationController.h"
#include "CharacterUI.h"

static const char* abilityUISlotNames[] =
{
    "Basic Attack",
    "Charged Attack",
    "Ability",
    "Dash"
};

constexpr int abilityUISlotCount = 4;

IMPLEMENT_SCRIPT_FIELDS(AbilityBase,
    SERIALIZED_ENUM_INT(m_uiSlot, "UI Slot", abilityUISlotNames, abilityUISlotCount)
)

AbilityBase::AbilityBase(GameObject* owner)
    : Script(owner)
{
}

void AbilityBase::Start()
{
    m_character = GameObjectAPI::findScript<CharacterBase>(getOwner());

    if (m_character == nullptr)
    {
        Debug::warn("[AbilityBase] CharacterBase not found on owner '%s'.", GameObjectAPI::getName(getOwner()));
    }

    m_characterUI = GameObjectAPI::findScript<CharacterUI>(getOwner());

    if (m_characterUI == nullptr)
    {
        Debug::warn("[AbilityBase] CharacterUI not found on owner '%s'.", GameObjectAPI::getName(getOwner()));
    }

    m_cooldownTimer.onCompleted([this]() {
        const AbilityUISlot slot = static_cast<AbilityUISlot>(m_uiSlot);
        if (m_characterUI) m_characterUI->hideAbilityCooldown(slot);
    });

    m_attackStateTimer.onCompleted([this]() {
        finishAttackWindow();
    });
}

void AbilityBase::Update()
{
    float dt = Time::getDeltaTime();

    m_cooldownTimer.update(dt);
    updateAttackWindow(dt);
    updateUI();
}

void AbilityBase::tryAbility()
{
    if (!canStartAbility())
    {
        return;
    }

    startAbility();
}

void AbilityBase::updateUI()
{
    if (!m_characterUI)
    {
        return;
    }

    const float cooldown = getCooldown();

    if (m_cooldownTimer.isReady() || cooldown <= 0.0001f)
    {
        return;
    }

    const AbilityUISlot slot = static_cast<AbilityUISlot>(m_uiSlot);
    m_characterUI->updateAbilityCooldown(slot, m_cooldownTimer.getRemaining() / cooldown);
}

void AbilityBase::reduceCooldown(float fraction)
{
    const float cooldown = getCooldown();

    if (!m_cooldownTimer.isActive() || fraction <= 0.0f || cooldown <= 0.0f)
    {
        return;
    }

    m_cooldownTimer.reduce(fraction * cooldown);

    if (m_cooldownTimer.isActive() && m_characterUI)
    {
        m_characterUI->updateAbilityCooldown(static_cast<AbilityUISlot>(m_uiSlot), m_cooldownTimer.getRemaining() / cooldown);
    }
}

void AbilityBase::startCooldown()
{
    m_cooldownTimer.start(getCooldown());

    if (m_characterUI)
    {
        m_characterUI->showAbilityCooldown(static_cast<AbilityUISlot>(m_uiSlot));
        m_characterUI->updateAbilityCooldown(static_cast<AbilityUISlot>(m_uiSlot), 1.0f);
    }
}

void AbilityBase::updateAttackWindow(float dt)
{
    if (!m_attackStateTimer.isActive())
    {
        return;
    }

    onAttackWindowUpdate();

    m_attackStateTimer.update(dt);
}

void AbilityBase::notifyAbilitySuccessfullyStarted()
{
    ++m_successfulUseCount;
}

bool AbilityBase::canStartAbility() const
{
    if (m_character == nullptr)
    {
        return false;
    }

    if (!m_isEnabled)
    {
        return false;
    }

    if (!isCooldownReady())
    {
        return false;
    }

    if (m_character->isDowned())
    {
        return false;
    }

    if (m_character->isUsingAbility())
    {
        return false;
    }

    if (!canStartSpecificAbility())
    {
        return false;
    }

    return true;
}

void AbilityBase::setAbilityLocked(bool locked) //innecesario
{
    if (m_character != nullptr)
    {
        m_character->setUsingAbility(locked);
    }
}

int AbilityBase::getPlayerIndex() const //innecesario
{
    if (m_character == nullptr)
    {
        return 0;
    }

    return m_character->getPlayerIndex();
}

void AbilityBase::beginAttackWindow(float lockDuration)
{
    m_attackStateTimer.start(lockDuration);
}

void AbilityBase::finishAttackWindow()
{
    m_attackStateTimer.stop();

    setAbilityLocked(false);

    if (m_character != nullptr)
    {
        PlayerState* playerState = m_character->getPlayerState();
        if (playerState != nullptr && playerState->isRecoveringAttack())
        {
            playerState->setState(PlayerStateType::Normal);
        }
    }

    onAttackWindowFinished();
}

void AbilityBase::beginAttackPresentation()
{
    if (m_character == nullptr)
    {
        return;
    }

    PlayerState* playerState = m_character->getPlayerState();
    if (playerState != nullptr)
    {
        if (playerState->isDowned())
        {
            return;
        }

        playerState->setState(PlayerStateType::AttackRecovery);
    }

    PlayerAnimationController* animController = m_character->getAnimationController();
    if (animController != nullptr)
    {
        animController->requestAttack();
    }
}

Vector3 AbilityBase::computeCameraRelativeAimDirection(float deadzoneSq) const //no me gustan transforms aqui
{
    const Vector2 lookAxis = Input::getLookAxis(getPlayerIndex());
    const float magSq = lookAxis.x * lookAxis.x + lookAxis.y * lookAxis.y;

    if (magSq < deadzoneSq)
    {
        return Vector3::Zero;
    }

    GameObject* cameraObject = SceneAPI::getDefaultCameraGameObject();
    if (cameraObject == nullptr)
    {
        Vector3 fallbackDirection(lookAxis.x, 0.0f, lookAxis.y);

        if (fallbackDirection.LengthSquared() > 0.0001f)
        {
            fallbackDirection.Normalize();
        }

        return fallbackDirection;
    }

    Transform* cameraTransform = GameObjectAPI::getTransform(cameraObject);
    if (cameraTransform == nullptr)
    {
        Vector3 fallbackDirection(lookAxis.x, 0.0f, lookAxis.y);

        if (fallbackDirection.LengthSquared() > 0.0001f)
        {
            fallbackDirection.Normalize();
        }

        return fallbackDirection;
    }

    Vector3 cameraForward = TransformAPI::getForward(cameraTransform);
    Vector3 cameraRight = TransformAPI::getRight(cameraTransform);

    cameraForward.y = 0.0f;
    cameraRight.y = 0.0f;

    if (cameraForward.LengthSquared() <= 0.0001f || cameraRight.LengthSquared() <= 0.0001f)
    {
        Vector3 fallbackDirection(lookAxis.x, 0.0f, lookAxis.y);

        if (fallbackDirection.LengthSquared() > 0.0001f)
        {
            fallbackDirection.Normalize();
        }

        return fallbackDirection;
    }

    cameraForward.Normalize();
    cameraRight.Normalize();

    Vector3 aimDirection = -cameraRight * lookAxis.x - cameraForward * lookAxis.y;

    if (aimDirection.LengthSquared() <= 0.0001f)
    {
        return Vector3::Zero;
    }

    aimDirection.Normalize();
    return aimDirection;
}

Vector3 AbilityBase::getFallbackFacingDirection() const
{
    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    if (ownerTransform == nullptr)
    {
        return Vector3::Zero;
    }

    Vector3 forward = TransformAPI::getForward(ownerTransform);
    forward.y = 0.0f;

    if (forward.LengthSquared() <= 0.0001f)
    {
        return Vector3::Zero;
    }

    forward.Normalize();
    return forward;
}

IMPLEMENT_SCRIPT(AbilityBase)

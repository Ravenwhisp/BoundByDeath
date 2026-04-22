#include "pch.h"
#include "AbilityBase.h"
#include "CharacterBase.h"

AbilityBase::AbilityBase(GameObject* owner)
    : Script(owner)
{
}

void AbilityBase::Start()
{
    m_character = findCharacterScript(getOwner());
}

void AbilityBase::Update()
{
    updateCooldown();
}

void AbilityBase::updateCooldown()
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

bool AbilityBase::canStartAbility() const
{
    if (!m_isEnabled)
    {
        return false;
    }

    if (m_character == nullptr)
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

    return true;
}

void AbilityBase::setAbilityLocked(bool locked)
{
    if (m_character != nullptr)
    {
        m_character->setUsingAbility(locked);
    }
}

int AbilityBase::getPlayerIndex() const
{
    if (m_character == nullptr)
    {
        return 0;
    }

    return m_character->getPlayerIndex();
}

CharacterBase* AbilityBase::findCharacterScript(GameObject* owner) const
{
    if (owner == nullptr)
    {
        return nullptr;
    }

    Script* script = GameObjectAPI::getScript(owner, "LyrielCharacter");
    if (script != nullptr)
    {
        return static_cast<CharacterBase*>(script);
    }

    script = GameObjectAPI::getScript(owner, "DeathCharacter");
    if (script != nullptr)
    {
        return static_cast<CharacterBase*>(script);
    }

    return nullptr;
}

Vector3 AbilityBase::computeCameraRelativeAimDirection(float deadzoneSq) const
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
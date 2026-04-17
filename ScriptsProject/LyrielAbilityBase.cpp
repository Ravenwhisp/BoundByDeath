#include "pch.h"
#include "LyrielAbilityBase.h"

#include "LyrielCharacter.h"
#include "PlayerRotation.h"
#include "PlayerState.h"

LyrielAbilityBase::LyrielAbilityBase(GameObject* owner)
    : AbilityBase(owner)
{
}

void LyrielAbilityBase::Start()
{
    m_lyriel = dynamic_cast<LyrielCharacter*>(GameObjectAPI::getScript(getOwner(), "LyrielCharacter"));

    m_character = m_lyriel;

    AbilityBase::Start();

    if (m_lyriel == nullptr)
    {
        Debug::log("[LyrielAbilityBase] LyrielCharacter not found on owner '%s'.", GameObjectAPI::getName(getOwner()));
    }
}

void LyrielAbilityBase::Update()
{
    AbilityBase::Update();
    updateAttackFacing();
    updateAttackStateTimer();
}

Transform* LyrielAbilityBase::findArrowSpawnTransform() const
{
    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    if (ownerTransform == nullptr || m_lyriel == nullptr)
    {
        return nullptr;
    }

    if (!m_lyriel->m_arrowSpawnChildName.empty())
    {
        Transform* spawnTransform = TransformAPI::findChildByName(
            ownerTransform,
            m_lyriel->m_arrowSpawnChildName.c_str());

        if (spawnTransform != nullptr)
        {
            return spawnTransform;
        }
    }

    return ownerTransform;
}

void LyrielAbilityBase::faceDirection(const Vector3& direction)
{
    if (m_character == nullptr)
    {
        return;
    }

    PlayerRotation* playerRotation = m_character->getPlayerRotation();
    if (playerRotation == nullptr)
    {
        return;
    }

    Vector3 flatDirection = direction;
    flatDirection.y = 0.0f;

    if (flatDirection.LengthSquared() <= 0.0001f)
    {
        return;
    }

    flatDirection.Normalize();
    playerRotation->applyFacingFromDirection(getOwner(), flatDirection, Time::getDeltaTime());
}

Vector3 LyrielAbilityBase::getFallbackFacingDirection() const
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

void LyrielAbilityBase::beginAttackLock(const Vector3& facingDirection, float lockDuration)
{
    m_attackFacingDirection = facingDirection;
    m_attackStateTimer = lockDuration;
}

void LyrielAbilityBase::updateAttackFacing()
{
    if (m_attackStateTimer > 0.0f)
    {
        if (m_attackFacingDirection.LengthSquared() > 0.0001f)
        {
            faceDirection(m_attackFacingDirection);
        }
    }
}

void LyrielAbilityBase::updateAttackStateTimer()
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

        setAbilityLocked(false);

        if (m_character != nullptr)
        {
            PlayerState* playerState = m_character->getPlayerState();
            if (playerState != nullptr && playerState->isAttacking())
            {
                playerState->setState(PlayerStateType::Normal);
            }
        }
    }
}
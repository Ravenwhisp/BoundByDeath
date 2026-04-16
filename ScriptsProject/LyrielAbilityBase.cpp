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
    m_lyriel = dynamic_cast<LyrielCharacter*>(
        GameObjectAPI::getScript(getOwner(), "LyrielCharacter"));

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

void LyrielAbilityBase::beginAttackLock(const Vector3& facingDirection)
{
    m_attackFacingDirection = facingDirection;
    m_attackStateTimer = m_attackLockDuration;
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
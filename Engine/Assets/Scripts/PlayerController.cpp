#include "pch.h"
#include "PlayerController.h"

#include "PlayerState.h"
#include "PlayerMovement.h"
#include "PlayerRotation.h"
#include "AbilityBase.h"

#include <cmath>

static const float PI = 3.1415926535897931f;

IMPLEMENT_SCRIPT_FIELDS(PlayerController,
    SERIALIZED_INT(m_playerIndex, "Player Index"),
    SERIALIZED_BOOL(m_godMode, "God Mode")
)

PlayerController::PlayerController(GameObject* owner)
    : Script(owner)
{
}

void PlayerController::Start()
{
    GameObject* owner   = getOwner();

    m_playerMovement    = findMovementScript(owner);
    m_playerRotation    = findRotationScript(owner);
    m_playerState       = findStateScript(owner);

    m_basicAttack       = findBasicAttackScript(owner);
    m_chargedAttack     = findChargedAttackScript(owner);
    m_dash              = findDashScript(owner);
    m_specialAbility    = findSpecialAbilityScript(owner);

    m_cameraTransform   = SceneAPI::getDefaultCameraGameObject() ?
        GameObjectAPI::getTransform(SceneAPI::getDefaultCameraGameObject()) :
        nullptr;

}

void PlayerController::Update()
{
    GameObject* owner = getOwner();
    if (!owner)
    {
        return;
    }

    const float dt = Time::getDeltaTime();

    if (m_playerState && !m_playerState->canMove())
    {
        if (m_playerMovement)
        {
            m_playerMovement->setMoving(false);
        }
        return;
    }

    if (m_playerMovement)
    {
        const Vector2 moveAxis = Input::getMoveAxis(m_playerIndex);

        Vector3 moveDirection = readMoveDirection(moveAxis);
        const bool isMoving = moveDirection.x != 0.0f || moveDirection.y != 0.0f || moveDirection.z != 0.0f;
        m_playerMovement->setMoving(isMoving);

        if (isMoving)
        {
            if (m_playerRotation)
            {
                Vector3 horizontalDir(moveDirection.x, 0.0f, moveDirection.z);
                if (horizontalDir.x != 0.0f || horizontalDir.z != 0.0f)
                {
                    horizontalDir.Normalize();
                    m_playerRotation->applyFacingFromDirection(owner, horizontalDir, dt);
                }
            }

            moveDirection.Normalize();
            m_playerMovement->playerMovement(owner, moveDirection * dt);
        }
    }

    if(m_basicAttack && Input::isRightShoulderJustPressed(m_playerIndex))
    {
		m_basicAttack->tryAbility();
    }

    if(m_chargedAttack && Input::isRightTriggerJustPressed(m_playerIndex))
    {
		m_chargedAttack->tryAbility();
	}

    if(m_dash && Input::isLeftShoulderJustPressed(m_playerIndex))
    {
		m_dash->tryAbility();
	}

    if (m_specialAbility && Input::isLeftTriggerJustPressed(m_playerIndex))
    {
		m_specialAbility->tryAbility();
    }

}

Vector3 PlayerController::getMoveDirection() const
{
    const Vector2 moveAxis = Input::getMoveAxis(m_playerIndex);
    return readMoveDirection(moveAxis);
}

Vector3 PlayerController::readMoveDirection(const Vector2& moveAxis) const
{
    Vector3 cameraForward = m_cameraTransform ? TransformAPI::getForward(m_cameraTransform) : Vector3(0.0f, 0.0f, 1.0f);
    Vector3 cameraRight = m_cameraTransform ? TransformAPI::getRight(m_cameraTransform) : Vector3(1.0f, 0.0f, 0.0f);
    Vector3 up = Vector3(0.0f, 1.0f, 0.0f);

    cameraForward = cameraForward - up * cameraForward.Dot(up);
    cameraRight = cameraRight - up * cameraRight.Dot(up);

    cameraForward.Normalize();
    cameraRight.Normalize();

    Vector3 move = Vector3(-moveAxis.x, 0.0f, -moveAxis.y);

    return cameraForward * move.z + cameraRight * move.x;
}

PlayerMovement* PlayerController::findMovementScript(GameObject* owner)
{
    Script* movementScript = owner ? GameObjectAPI::getScript(owner, "PlayerMovement") : nullptr;
    if (movementScript)
    {
        return static_cast<PlayerMovement*>(movementScript);
    }

    Debug::warn("PlayerController on '%s' could not find PlayerMovement on the same GameObject.",
        GameObjectAPI::getName(owner));
    return nullptr;
}

PlayerRotation* PlayerController::findRotationScript(GameObject* owner)
{
    Script* rotationScript = owner ? GameObjectAPI::getScript(owner, "PlayerRotation") : nullptr;
    if (rotationScript)
    {
        return static_cast<PlayerRotation*>(rotationScript);
    }

    Debug::warn("PlayerController on '%s' could not find PlayerRotation on the same GameObject.",
        GameObjectAPI::getName(owner));
    return nullptr;
}

PlayerState* PlayerController::findStateScript(GameObject* owner)
{
    Script* stateScript = owner ? GameObjectAPI::getScript(owner, "PlayerState") : nullptr;
    if (stateScript)
    {
        return static_cast<PlayerState*>(stateScript);
    }

    Debug::warn("PlayerController on '%s' could not find PlayerState on the same GameObject.",
        GameObjectAPI::getName(owner));
    return nullptr;
}

AbilityBase* PlayerController::findBasicAttackScript(GameObject* owner)
{
	Script* basicAttackScript = owner ? GameObjectAPI::getScript(owner, "LyrielBasicAttack") : nullptr;
    if (basicAttackScript)
    {
        return static_cast<AbilityBase*>(basicAttackScript);
    }
    
	basicAttackScript = owner ? GameObjectAPI::getScript(owner, "DeathBasicAttack") : nullptr;
    if (basicAttackScript)
    {
        return static_cast<AbilityBase*>(basicAttackScript);
	}
    Debug::warn("PlayerController on '%s' could not find LyrielBasicAttack or DeathBasicAttack on the same GameObject.",
        GameObjectAPI::getName(owner));
    return nullptr;
}

AbilityBase* PlayerController::findChargedAttackScript(GameObject* owner)
{
    Script* chargedAttackScript = owner ? GameObjectAPI::getScript(owner, "LyrielChargedAttack") : nullptr;
    if (chargedAttackScript)
    {
        return static_cast<AbilityBase*>(chargedAttackScript);
    }
    
    chargedAttackScript = owner ? GameObjectAPI::getScript(owner, "DeathChargedAttack") : nullptr;
    if (chargedAttackScript)
    {
        return static_cast<AbilityBase*>(chargedAttackScript);
    }
    Debug::warn("PlayerController on '%s' could not find LyrielChargedAttack or DeathChargedAttack on the same GameObject.",
        GameObjectAPI::getName(owner));
	return nullptr;
}

AbilityBase* PlayerController::findDashScript(GameObject* owner)
{
    Script* dashScript = owner ? GameObjectAPI::getScript(owner, "LyrielDash") : nullptr;
    if (dashScript)
    {
        return static_cast<AbilityBase*>(dashScript);
    }
    
    dashScript = owner ? GameObjectAPI::getScript(owner, "DeathDash") : nullptr;
    if (dashScript)
    {
        return static_cast<AbilityBase*>(dashScript);
    }
    Debug::warn("PlayerController on '%s' could not find LyrielDash or DeathDash on the same GameObject.",
        GameObjectAPI::getName(owner));
	return nullptr;
}

AbilityBase* PlayerController::findSpecialAbilityScript(GameObject* owner)
{
    Script* specialAbilityScript = owner ? GameObjectAPI::getScript(owner, "LyrielArrowVolley") : nullptr;
    if (specialAbilityScript)
    {
        return static_cast<AbilityBase*>(specialAbilityScript);
    }
    
    specialAbilityScript = owner ? GameObjectAPI::getScript(owner, "DeathTaunt") : nullptr;
    if (specialAbilityScript)
    {
        return static_cast<AbilityBase*>(specialAbilityScript);
    }
    Debug::warn("PlayerController on '%s' could not find LyrielArrowVolley or DeathTaunt on the same GameObject.",
        GameObjectAPI::getName(owner));
	return nullptr;
}

IMPLEMENT_SCRIPT(PlayerController)
#include "pch.h"
#include "PlayerController.h"

#include "PlayerMovement.h"
#include "PlayerRotation.h"
#include "AbilityBase.h"

static const ScriptFieldInfo playerControllerFields[] =
{
    { "Player Index", ScriptFieldType::Int, offsetof(PlayerController, m_playerIndex) }, // 0 keyboard, 1 gamepad
    { "God Mode", ScriptFieldType::Bool, offsetof(PlayerController, m_godMode) }
};

IMPLEMENT_SCRIPT_FIELDS(PlayerController, playerControllerFields)

PlayerController::PlayerController(GameObject* owner)
    : Script(owner)
{
}

void PlayerController::Start()
{
    GameObject* owner = getOwner();

	m_movement = findMovementScript(owner);
	m_rotation = findRotationScript(owner);

	m_dash = findDashScript(owner);
	m_basicAttack = findBasicAttackScript(owner);
	m_chargedAttack = findChargedAttackScript(owner);
	m_specialAbility = findSpecialAbilityScript(owner);

    m_cameraTransform = SceneAPI::getDefaultCameraGameObject() ?
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

  //  Vector3 direction = readMoveDirection();
  //  const float dt = Time::getDeltaTime();

  //  if (m_dash && Input::isLeftShoulderJustPressed() && m_dash->canDash())
  //  {
		////faltaria pasar la direccion, que si es 0 es hacia atras, pasar tambien tiempo...
  //      m_dash->startDash(direction);
  //  }
  //  //ataques
  //  if (m_movement)
  //  {
  //      if (direction.x != 0.0f || direction.y != 0.0f || direction.z != 0.0f)
  //      {
  //          bool shiftHeld = Input::isKeyDown(KeyCode::LeftShift) || Input::isKeyDown(KeyCode::RightShift);
  //          if (m_rotation)
  //          {
  //              Vector3 horizontalDir(direction.x, 0.0f, direction.z);
  //              if (horizontalDir.x != 0.0f || horizontalDir.z != 0.0f)
  //              {
  //                  horizontalDir.Normalize();
  //                  m_rotation->applyFacingFromDirection(owner, horizontalDir, dt);
  //              }
  //          }
  //          direction.Normalize();
  //          m_movement->normalMovement(owner, direction * dt, shiftHeld);
		//}
  //  }

    const Vector2 moveAxis = Input::getMoveAxis(m_playerIndex);

	const bool dashPressed = Input::isLeftShoulderJustPressed(m_playerIndex);
	const bool specialPressed = Input::isRightTriggerJustPressed(m_playerIndex);
	const bool basicAttackPressed = Input::isRightShoulderJustPressed(m_playerIndex);
	const bool chargedAttackPressed = Input::isLeftTriggerPressed(m_playerIndex);

	const Vector3 moveDirection = readMoveDirection(moveAxis);
	const bool isMoving = moveDirection.x != 0.0f || moveDirection.y != 0.0f || moveDirection.z != 0.0f;

    if(dashPressed && m_dash && m_dash->canActivate())
    {
        m_dash->onActivate();
	}

    if(specialPressed && m_specialAbility && m_specialAbility->canActivate())
    {
        m_specialAbility->onActivate();
	}

    if (chargedAttackPressed && m_chargedAttack && m_chargedAttack->canActivate())
    {
        m_chargedAttack->onActivate(); //esto cambiará, porque el charged attack no se activa al pulsar, sino al soltar el trigger, pero así se prueba la lógica de input
    }

    if(basicAttackPressed && m_basicAttack && m_basicAttack->canActivate())
    {
        m_basicAttack->onActivate();
	}

    //movement
}

PlayerMovement* PlayerController::findMovementScript(GameObject* owner)
{
    Script* movementScript = owner ? GameObjectAPI::getScript(owner, "PlayerMovement") : nullptr;
    if (movementScript)
    {
        return static_cast<PlayerMovement*>(movementScript);
    }
    
    Debug::warn("PlayerController on '%s' could not find PlayerMovement on the same GameObject.",
            GameObjectAPI::getName(m_owner));
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
            GameObjectAPI::getName(m_owner));
	return nullptr;
}

AbilityBase* PlayerController::findDashScript(GameObject* owner)
{
    Script* dashScript = owner ? GameObjectAPI::getScript(owner, "LyrielDash") : nullptr;
    if (!dashScript)
    {
        dashScript = owner ? GameObjectAPI::getScript(owner, "DeathDash") : nullptr;
    }

    if (dashScript)
    {
        return static_cast<AbilityBase*>(dashScript);
    }

    Debug::warn("PlayerController on '%s' could not find any dash implementation on the same GameObject.",
            GameObjectAPI::getName(m_owner));
	return nullptr;
}

AbilityBase* PlayerController::findBasicAttackScript(GameObject* owner)
{
	Script* basicAttackScript = owner ? GameObjectAPI::getScript(owner, "LyrielBasicAttack") : nullptr;
    if(!basicAttackScript)
    {
        basicAttackScript = owner ? GameObjectAPI::getScript(owner, "DeathBasicAttack") : nullptr;
	}

    if(basicAttackScript)
    {
        return static_cast<AbilityBase*>(basicAttackScript);
	}

    Debug::warn("PlayerController on '%s' could not find any basic attack implementation on the same GameObject.",
		GameObjectAPI::getName(m_owner));
	return nullptr;
}

AbilityBase* PlayerController::findChargedAttackScript(GameObject* owner)
{
    Script* chargedAttackScript = owner ? GameObjectAPI::getScript(owner, "LyrielChargedAttack") : nullptr;
    if(!chargedAttackScript)
    {
        chargedAttackScript = owner ? GameObjectAPI::getScript(owner, "DeathChargedAttack") : nullptr;
    }

    if(chargedAttackScript)
    {
        return static_cast<AbilityBase*>(chargedAttackScript);
    }

    Debug::warn("PlayerController on '%s' could not find any charged attack implementation on the same GameObject.",
        GameObjectAPI::getName(m_owner));
	return nullptr;
}

AbilityBase* PlayerController::findSpecialAbilityScript(GameObject* owner)
{
    Script* specialAbilityScript = owner ? GameObjectAPI::getScript(owner, "LyrielArrowVolley") : nullptr;
    if(!specialAbilityScript)
    {
        specialAbilityScript = owner ? GameObjectAPI::getScript(owner, "DeathTaunt") : nullptr;
    }

    if(specialAbilityScript)
    {
        return static_cast<AbilityBase*>(specialAbilityScript);
    }

    Debug::warn("PlayerController on '%s' could not find any special ability implementation on the same GameObject.",
        GameObjectAPI::getName(m_owner));
	return nullptr;
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

IMPLEMENT_SCRIPT(PlayerController)
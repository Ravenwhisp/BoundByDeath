#include "pch.h"
#include "PlayerController.h"

#include "PlayerMovement.h"
#include "PlayerRotation.h"

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

    const char* ownerName = GameObjectAPI::getName(m_owner);

    Script* movementScript = owner ? GameObjectAPI::getScript(owner, "PlayerMovement") : nullptr;
    if (movementScript)
    {
        m_movement = static_cast<PlayerMovement*>(movementScript);
    }
    else
    {
        Debug::warn("PlayerController on '%s' could not find PlayerMovement on the same GameObject.", 
            ownerName);
    }

    Script* rotationScript = owner ? GameObjectAPI::getScript(owner, "PlayerRotation") : nullptr;
    if (rotationScript)
    {
        m_rotation = static_cast<PlayerRotation*>(rotationScript);
    }
    else
    {
        Debug::warn("PlayerController on '%s' could not find PlayerRotation on the same GameObject.",
            ownerName);
    }

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

    if (m_movement) //no pongo early return porque en teoria habran mas cosas que tener en cuenta
    {
        Vector3 direction = readMoveDirection();

        if (direction.x != 0.0f || direction.y != 0.0f || direction.z != 0.0f)
        {
            const float dt = Time::getDeltaTime();
            bool shiftHeld = Input::isKeyDown(KeyCode::LeftShift) || Input::isKeyDown(KeyCode::RightShift);

            if (m_rotation)
            {
                Vector3 horizontalDir(direction.x, 0.0f, direction.z);
                if (horizontalDir.x != 0.0f || horizontalDir.z != 0.0f)
                {
                    horizontalDir.Normalize();
                    m_rotation->applyFacingFromDirection(owner, horizontalDir, dt);
                }
            }

            direction.Normalize();
            m_movement->applyTranslation(owner, direction, dt, shiftHeld);
        }
    }
}

Vector3 PlayerController::readMoveDirection() const
{
    const Vector2 moveAxis = Input::getMoveAxis(m_playerIndex);

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
#include "pch.h"
#include "DeathDash.h"
#include "CharacterBase.h"
#include "PlayerTargetController.h"

#define PI 3.1415926535897931f

static const ScriptFieldInfo DeathDashFields[] =
{
    { "Dash Distance", ScriptFieldType::Float, offsetof(DeathDash, m_dashDistance), { 0.1f, 20.0f, 0.1f } },
    { "Dash Duration", ScriptFieldType::Float, offsetof(DeathDash, m_dashDuration), { 0.05f, 1.0f, 0.01f } },
};

IMPLEMENT_SCRIPT_FIELDS(DeathDash, DeathDashFields)

DeathDash::DeathDash(GameObject* owner)
    : AbilityBase(owner)
{
    m_cooldown = 0.5f;
}

void DeathDash::Start()
{
    m_character = static_cast<CharacterBase*>(GameObjectAPI::getScript(m_owner, "DeathCharacter"));
    if (m_character == nullptr)
    {
        Debug::warn("DeathDash: DeathCharacter not found on this GameObject.");
    }
}

void DeathDash::Update()
{
    AbilityBase::Update();

    if (m_character == nullptr)
    {
        return;
    }

    // =========================================================
    // ACTIVACION DEL DASH
    // PRIORIDAD:
    // 1) direccion del joystick izquierdo / move axis
    // 2) enemigo targeteado
    // 3) forward del personaje
    // =========================================================

    const bool testKeyDown = Input::isKeyDown(KeyCode::Space);
    const bool testKeyJustPressed = testKeyDown && !m_debugDashKeyWasDown;
    m_debugDashKeyWasDown = testKeyDown;

    // Para mando, descomenta esta linea:
    // const bool dashJustPressed = Input::isLeftShoulderJustPressed(getPlayerIndex());

    // Para probar con teclado:
    const bool dashJustPressed = testKeyJustPressed;

    if (!isActive() && canActivate() && dashJustPressed)
    {
        m_dashTimer = 0.0f;

        Transform* ownerTransform = GameObjectAPI::getTransform(m_owner);
        if (ownerTransform == nullptr)
        {
            return;
        }

        Vector3 dashDirection = { 0.0f, 0.0f, 0.0f };

        // 1) Si hay input de movimiento, usar esa direccion
        Vector2 moveAxis = Input::getMoveAxis(getPlayerIndex());
        if (moveAxis.x != 0.0f || moveAxis.y != 0.0f)
        {
            dashDirection = Vector3(moveAxis.x, 0.0f, moveAxis.y);
        }
        else
        {
            // 2) Si no hay input, intentar ir hacia el target actual
            PlayerTargetController* targetController = m_character->getTargetController();
            GameObject* currentTarget = nullptr;

            if (targetController != nullptr)
            {
                currentTarget = targetController->getCurrentTarget();
            }

            if (currentTarget != nullptr)
            {
                Transform* targetTransform = GameObjectAPI::getTransform(currentTarget);
                if (targetTransform != nullptr)
                {
                    Vector3 ownerPos = TransformAPI::getPosition(ownerTransform);
                    Vector3 targetPos = TransformAPI::getPosition(targetTransform);

                    dashDirection = targetPos - ownerPos;
                    dashDirection.y = 0.0f;
                }
            }
            else
            {
                // 3) Si no hay input ni target, hacia adelante
                dashDirection = -TransformAPI::getForward(ownerTransform);
                dashDirection.y = 0.0f;
            }
        }

        float lenSq =
            dashDirection.x * dashDirection.x +
            dashDirection.y * dashDirection.y +
            dashDirection.z * dashDirection.z;

        if (lenSq <= 0.0001f)
        {
            dashDirection = -TransformAPI::getForward(ownerTransform);
            dashDirection.y = 0.0f;

            lenSq =
                dashDirection.x * dashDirection.x +
                dashDirection.y * dashDirection.y +
                dashDirection.z * dashDirection.z;
        }

        if (lenSq <= 0.0001f)
        {
            Debug::warn("DeathDash: invalid dash direction.");
            return;
        }

        const float invLen = 1.0f / sqrtf(lenSq);
        m_dashDirection = dashDirection * invLen;

        Debug::log("DeathDash activated.");
        onActivate();
        return;
    }

    // =========================================================
    // MOVIMIENTO DEL DASH
    // =========================================================
    if (isActive())
    {
        const float dt = Time::getDeltaTime();
        m_dashTimer += dt;

        const float rawT = m_dashTimer / m_dashDuration;
        const float t = (rawT < 0.0f) ? 0.0f : (rawT > 1.0f ? 1.0f : rawT);

        const float curveSpeed = 0.5f * PI * cosf(t * PI * 0.5f);
        const float dashSpeed = (m_dashDistance / m_dashDuration) * curveSpeed;

        Transform* ownerTransform = GameObjectAPI::getTransform(m_owner);
        if (ownerTransform != nullptr)
        {
            const Vector3 delta = m_dashDirection * dashSpeed * dt;
            TransformAPI::translate(ownerTransform, delta);
        }

        if (m_dashTimer >= m_dashDuration)
        {
            onDeactivate();
            Debug::log("DeathDash finished.");
        }
    }
}

IMPLEMENT_SCRIPT(DeathDash)
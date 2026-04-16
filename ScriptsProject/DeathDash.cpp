#include "pch.h"
#include "DeathDash.h"
#include "CharacterBase.h"
#include "PlayerTargetController.h"
#include "Damageable.h"

#define PI 3.1415926535897931f

static const ScriptFieldInfo DeathDashFields[] =
{
    { "Dash Distance", ScriptFieldType::Float, offsetof(DeathDash, m_dashDistance), { 0.1f, 20.0f, 0.1f } },
    { "Dash Duration", ScriptFieldType::Float, offsetof(DeathDash, m_dashDuration), { 0.05f, 1.0f, 0.01f } },
    { "Dash Damage", ScriptFieldType::Float, offsetof(DeathDash, m_dashDamage), { 0.0f, 999.0f, 1.0f } },
    { "Hit Radius", ScriptFieldType::Float, offsetof(DeathDash, m_hitRadius), { 0.1f, 5.0f, 0.05f } },
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
        m_hitEnemiesThisDash.clear();

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
            dashDirection = Vector3(-moveAxis.x, 0.0f, -moveAxis.y);
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
                dashDirection = TransformAPI::getForward(ownerTransform);
                dashDirection.y = 0.0f;
            }
        }

        float lenSq =
            dashDirection.x * dashDirection.x +
            dashDirection.y * dashDirection.y +
            dashDirection.z * dashDirection.z;

        if (lenSq <= 0.0001f)
        {
            dashDirection = TransformAPI::getForward(ownerTransform);
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
        Transform* ownerTransform = GameObjectAPI::getTransform(m_owner);
        if (ownerTransform == nullptr)
        {
            onDeactivate();
            return;
        }

        const Vector3 previousPosition = TransformAPI::getPosition(ownerTransform);

        const float dt = Time::getDeltaTime();
        m_dashTimer += dt;

        const float rawT = m_dashTimer / m_dashDuration;
        const float t = (rawT < 0.0f) ? 0.0f : (rawT > 1.0f ? 1.0f : rawT);

        const float curveSpeed = 0.5f * PI * cosf(t * PI * 0.5f);
        const float dashSpeed = (m_dashDistance / m_dashDuration) * curveSpeed;

        const Vector3 delta = m_dashDirection * dashSpeed * dt;
        TransformAPI::translate(ownerTransform, delta);

        const Vector3 currentPosition = TransformAPI::getPosition(ownerTransform);
        checkDashHits(previousPosition, currentPosition);

        if (m_dashTimer >= m_dashDuration)
        {
            onDeactivate();
            Debug::log("DeathDash finished.");
        }
    }
}

void DeathDash::checkDashHits(const Vector3& previousPosition, const Vector3& currentPosition)
{
    const std::vector<GameObject*> enemies = SceneAPI::findAllGameObjectsByTag(Tag::ENEMY, true);
    const float hitRadiusSq = m_hitRadius * m_hitRadius;

    for (GameObject* enemy : enemies)
    {
        if (enemy == nullptr)
        {
            continue;
        }

        if (hasAlreadyHit(enemy))
        {
            continue;
        }

        Transform* enemyTransform = GameObjectAPI::getTransform(enemy);
        if (enemyTransform == nullptr)
        {
            continue;
        }

        const Vector3 enemyPosition = TransformAPI::getPosition(enemyTransform);
        const float distanceSq = distancePointToSegmentSquaredXZ(enemyPosition, previousPosition, currentPosition);

        if (distanceSq > hitRadiusSq)
        {
            continue;
        }

        Script* damageableScript = GameObjectAPI::getScript(enemy, "Damageable");
        if (damageableScript == nullptr)
        {
            continue;
        }

        Damageable* damageable = static_cast<Damageable*>(damageableScript);
        damageable->takeDamage(m_dashDamage);

        m_hitEnemiesThisDash.push_back(enemy);

        Debug::log("DeathDash hit enemy for %.2f damage.", m_dashDamage);
    }
}

bool DeathDash::hasAlreadyHit(GameObject* enemy) const
{
    for (GameObject* hitEnemy : m_hitEnemiesThisDash)
    {
        if (hitEnemy == enemy)
        {
            return true;
        }
    }

    return false;
}

float DeathDash::distancePointToSegmentSquaredXZ(const Vector3& point, const Vector3& segmentStart, const Vector3& segmentEnd)
{
    const float abx = segmentEnd.x - segmentStart.x;
    const float abz = segmentEnd.z - segmentStart.z;

    const float apx = point.x - segmentStart.x;
    const float apz = point.z - segmentStart.z;

    const float abLenSq = abx * abx + abz * abz;

    if (abLenSq <= 0.000001f)
    {
        const float dx = point.x - segmentStart.x;
        const float dz = point.z - segmentStart.z;
        return dx * dx + dz * dz;
    }

    float t = (apx * abx + apz * abz) / abLenSq;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    const float closestX = segmentStart.x + abx * t;
    const float closestZ = segmentStart.z + abz * t;

    const float dx = point.x - closestX;
    const float dz = point.z - closestZ;

    return dx * dx + dz * dz;
}

IMPLEMENT_SCRIPT(DeathDash)
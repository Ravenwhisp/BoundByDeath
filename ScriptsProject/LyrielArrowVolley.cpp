#include "pch.h"
#include "LyrielArrowVolley.h"

#include "ArrowPool.h"
#include "LyrielArrowProjectile.h"
#include "Damageable.h"
#include "PlayerState.h"
#include "PlayerRotation.h"
#include "PlayerAnimationController.h"

#include <cmath>

static const ScriptFieldInfo LyrielArrowVolleyFields[] =
{
    { "Player Index", ScriptFieldType::Int, offsetof(LyrielArrowVolley, m_playerIndex) },
    { "Volley Damage", ScriptFieldType::Float, offsetof(LyrielArrowVolley, m_volleyDamage), { 0.0f, 100.0f, 0.5f } },
    { "Volley Cooldown", ScriptFieldType::Float, offsetof(LyrielArrowVolley, m_volleyCooldown), { 0.0f, 20.0f, 0.1f } },
    { "Volley Range", ScriptFieldType::Float, offsetof(LyrielArrowVolley, m_volleyRange), { 0.0f, 50.0f, 0.1f } },
    { "Cone Angle Degrees", ScriptFieldType::Float, offsetof(LyrielArrowVolley, m_coneAngleDegrees), { 1.0f, 180.0f, 1.0f } },
    { "Num Visual Arrows", ScriptFieldType::Int, offsetof(LyrielArrowVolley, m_numVisualArrows), { 1.0f, 20.0f, 1.0f } },
    { "Arrow Speed", ScriptFieldType::Float, offsetof(LyrielArrowVolley, m_arrowSpeed), { 0.0f, 100.0f, 0.5f } },
    { "Attack Lock Duration", ScriptFieldType::Float, offsetof(LyrielArrowVolley, m_attackLockDuration), { 0.0f, 2.0f, 0.01f } },
    { "Arrow Spawn Child Name", ScriptFieldType::String, offsetof(LyrielArrowVolley, m_arrowSpawnChildName) }
};

IMPLEMENT_SCRIPT_FIELDS(LyrielArrowVolley, LyrielArrowVolleyFields)

LyrielArrowVolley::LyrielArrowVolley(GameObject* owner)
    : Script(owner)
{
}

void LyrielArrowVolley::Start()
{
    Script* arrowPoolScript = GameObjectAPI::getScript(getOwner(), "ArrowPool");
    m_arrowPool = dynamic_cast<ArrowPool*>(arrowPoolScript);

    Script* stateScript = GameObjectAPI::getScript(getOwner(), "PlayerState");
    m_playerState = dynamic_cast<PlayerState*>(stateScript);

    Script* rotationScript = GameObjectAPI::getScript(getOwner(), "PlayerRotation");
    m_playerRotation = dynamic_cast<PlayerRotation*>(rotationScript);

    Script* animationScript = GameObjectAPI::getScript(getOwner(), "PlayerAnimationController");
    m_playerAnimationController = dynamic_cast<PlayerAnimationController*>(animationScript);

    if (m_arrowPool == nullptr)
    {
        Debug::log("[LyrielArrowVolley] ArrowPool not found on owner.");
    }

    if (m_playerState == nullptr)
    {
        Debug::log("[LyrielArrowVolley] PlayerState not found on owner.");
    }

    if (m_playerRotation == nullptr)
    {
        Debug::log("[LyrielArrowVolley] PlayerRotation not found on owner.");
    }

    if (m_playerAnimationController == nullptr)
    {
        Debug::log("[LyrielArrowVolley] PlayerAnimationController not found on owner.");
    }
}

void LyrielArrowVolley::Update()
{
    updateCooldown();

    if (m_attackStateTimer > 0.0f)
    {
        if (m_attackFacingDirection.LengthSquared() > 0.0001f)
        {
            faceDirection(m_attackFacingDirection);
        }
    }

    updateAttackStateTimer();

    if (canStartAim() && Input::isLeftTriggerJustPressed(m_playerIndex))
    {
        beginAim();
    }

    if (m_isAiming && Input::isLeftTriggerPressed(m_playerIndex))
    {
        updateAim();
    }

    if (m_isAiming && Input::isLeftTriggerReleased(m_playerIndex))
    {
        releaseAimAndCast();
    }
}

void LyrielArrowVolley::drawGizmo()
{
    if (!m_isAiming)
    {
        return;
    }

    if (m_currentAimDirection.LengthSquared() <= 0.0001f)
    {
        return;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());

    const Vector3 origin = TransformAPI::getGlobalPosition(ownerTransform);
    drawAimPreview(origin, m_currentAimDirection);
}

void LyrielArrowVolley::updateCooldown()
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

void LyrielArrowVolley::updateAttackStateTimer()
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

        if (m_playerState != nullptr && m_playerState->isAttacking())
        {
            m_playerState->setState(PlayerStateType::Normal);
        }
    }
}

bool LyrielArrowVolley::canStartAim() const
{
    if (m_cooldownTimer > 0.0f)
    {
        return false;
    }

    if (m_playerState != nullptr && m_playerState->isDowned())
    {
        return false;
    }

    return true;
}

bool LyrielArrowVolley::canCast() const
{
    if (m_arrowPool == nullptr)
    {
        return false;
    }

    if (m_playerState != nullptr && m_playerState->isDowned())
    {
        return false;
    }

    return true;
}

void LyrielArrowVolley::beginAim()
{
    m_isAiming = true;
    m_currentAimDirection = Vector3::Zero;

    Vector3 aimDirection = computeAimDirection();
    if (isAimStickValid(aimDirection))
    {
        m_currentAimDirection = aimDirection;
    }
}

void LyrielArrowVolley::updateAim()
{
    Vector3 aimDirection = computeAimDirection();
    if (isAimStickValid(aimDirection))
    {
        m_currentAimDirection = aimDirection;
    }
}

void LyrielArrowVolley::releaseAimAndCast()
{
    m_isAiming = false;

    if (!canCast())
    {
        return;
    }

    if (m_currentAimDirection.LengthSquared() <= 0.0001f)
    {
        return;
    }

    Transform* spawnTransform = findArrowSpawnTransform();
    if (spawnTransform == nullptr)
    {
        return;
    }

    const Vector3 origin = TransformAPI::getGlobalPosition(spawnTransform);
    const Vector3 forward = m_currentAimDirection;

    m_attackFacingDirection = forward;
    faceDirection(forward);

    std::vector<GameObject*> targets;
    collectEnemiesInCone(origin, forward, targets);
    applyVolleyDamage(targets);
    spawnVolleyArrows(origin, forward);

    if (m_playerState != nullptr)
    {
        m_playerState->setState(PlayerStateType::Attacking);
    }

    if (m_playerAnimationController != nullptr)
    {
        m_playerAnimationController->requestAttack();
    }

    m_attackStateTimer = m_attackLockDuration;
    m_cooldownTimer = m_volleyCooldown;

    Debug::log("[LyrielArrowVolley] Cast Arrow Volley. Targets hit: %d", static_cast<int>(targets.size()));
}

Transform* LyrielArrowVolley::findArrowSpawnTransform() const
{
    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());

    if (ownerTransform == nullptr)
    {
        return nullptr;
    }

    if (!m_arrowSpawnChildName.empty())
    {
        Transform* spawnTransform = TransformAPI::findChildByName(ownerTransform, m_arrowSpawnChildName.c_str());
        if (spawnTransform != nullptr)
        {
            return spawnTransform;
        }
    }

    return ownerTransform;
}

Vector3 LyrielArrowVolley::computeAimDirection() const
{
    const Vector2 lookAxis = Input::getLookAxis(m_playerIndex);
    return Vector3(lookAxis.x, 0.0f, lookAxis.y);
}

void LyrielArrowVolley::faceDirection(const Vector3& direction)
{
    if (m_playerRotation == nullptr)
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
    m_playerRotation->applyFacingFromDirection(getOwner(), flatDirection, Time::getDeltaTime());
}

bool LyrielArrowVolley::isAimStickValid(const Vector3& direction) const
{
    Vector3 flatDirection = direction;
    flatDirection.y = 0.0f;

    return flatDirection.LengthSquared() > 0.0001f;
}

void LyrielArrowVolley::collectEnemiesInCone(const Vector3& origin, const Vector3& forward, std::vector<GameObject*>& outTargets)
{
    outTargets.clear();

    std::vector<GameObject*> allEnemies = SceneAPI::findAllGameObjectsByTag(Tag::ENEMY, true);

    Vector3 flatForward = forward;
    flatForward.y = 0.0f;

    if (flatForward.LengthSquared() <= 0.0001f)
    {
        return;
    }

    flatForward.Normalize();

    const float halfAngleRadians = DirectX::XMConvertToRadians(m_coneAngleDegrees * 0.5f);
    const float minDot = std::cos(halfAngleRadians);

    for (GameObject* enemy : allEnemies)
    {
        if (enemy == nullptr)
        {
            continue;
        }

        Transform* enemyTransform = GameObjectAPI::getTransform(enemy);
        if (enemyTransform == nullptr)
        {
            continue;
        }

        Vector3 enemyPosition = TransformAPI::getGlobalPosition(enemyTransform);
        Vector3 toEnemy = enemyPosition - origin;
        toEnemy.y = 0.0f;

        const float distanceSq = toEnemy.LengthSquared();
        if (distanceSq <= 0.0001f)
        {
            continue;
        }

        if (distanceSq > (m_volleyRange * m_volleyRange))
        {
            continue;
        }

        toEnemy.Normalize();

        const float dot = flatForward.Dot(toEnemy);
        if (dot >= minDot)
        {
            outTargets.push_back(enemy);
        }
    }
}

void LyrielArrowVolley::applyVolleyDamage(const std::vector<GameObject*>& targets)
{
    for (GameObject* target : targets)
    {
        if (target == nullptr)
        {
            continue;
        }

        Script* script = GameObjectAPI::getScript(target, "Damageable");
        Damageable* damageable = dynamic_cast<Damageable*>(script);

        if (damageable != nullptr)
        {
            damageable->takeDamage(m_volleyDamage);
        }
    }
}

void LyrielArrowVolley::spawnVolleyArrows(const Vector3& origin, const Vector3& forward)
{
    if (m_arrowPool == nullptr || m_numVisualArrows <= 0)
    {
        return;
    }

    Vector3 flatForward = forward;
    flatForward.y = 0.0f;

    if (flatForward.LengthSquared() <= 0.0001f)
    {
        return;
    }

    flatForward.Normalize();

    const float totalAngle = m_coneAngleDegrees;
    float lifetime = 0.0f;
    if  (m_arrowSpeed > 0.0001f) 
    {
        lifetime = m_volleyRange / m_arrowSpeed;
    } 

    for (int i = 0; i < m_numVisualArrows; ++i)
    {
        LyrielArrowProjectile* arrow = m_arrowPool->acquireArrow();
        if (arrow == nullptr)
        {
            Debug::log("[LyrielArrowVolley] No available arrow in pool for visual arrow %d.", i);
            return;
        }

        float t = 0.5f;
        if (m_numVisualArrows > 1)
        {
            t = static_cast<float>(i) / static_cast<float>(m_numVisualArrows - 1);
        }

        const float angleOffset = -totalAngle * 0.5f + totalAngle * t;
        const float angleRad = DirectX::XMConvertToRadians(angleOffset);

        const float cosA = std::cos(angleRad);
        const float sinA = std::sin(angleRad);

        Vector3 dir;
        dir.x = flatForward.x * cosA - flatForward.z * sinA;
        dir.y = 0.0f;
        dir.z = flatForward.x * sinA + flatForward.z * cosA;

        if (dir.LengthSquared() <= 0.0001f)
        {
            dir = flatForward;
        }
        else
        {
            dir.Normalize();
        }

        arrow->launch(origin, dir, m_arrowSpeed, lifetime, nullptr, 0.0f);
    }
}

void LyrielArrowVolley::drawAimPreview(const Vector3& origin, const Vector3& forward) const
{
    Vector3 flatForward = forward;
    flatForward.y = 0.0f;

    if (flatForward.LengthSquared() <= 0.0001f)
    {
        return;
    }

    flatForward.Normalize();

    const float halfAngleRad = DirectX::XMConvertToRadians(m_coneAngleDegrees * 0.5f);
    const int arcSteps = 16;

    const Vector3 previewColor(0.2f, 1.0f, 0.2f);

    auto rotateXZ = [](const Vector3& dir, float angleRad) -> Vector3
        {
            const float c = std::cos(angleRad);
            const float s = std::sin(angleRad);

            Vector3 result;
            result.x = dir.x * c - dir.z * s;
            result.y = 0.0f;
            result.z = dir.x * s + dir.z * c;
            return result;
        };

    Vector3 leftDir = rotateXZ(flatForward, -halfAngleRad);
    Vector3 rightDir = rotateXZ(flatForward, halfAngleRad);

    leftDir.Normalize();
    rightDir.Normalize();

    DebugDrawAPI::drawLine(origin, origin + leftDir * m_volleyRange, previewColor, 0, true);
    DebugDrawAPI::drawLine(origin, origin + rightDir * m_volleyRange, previewColor, 0, true);

    Vector3 previousPoint = origin + leftDir * m_volleyRange;

    for (int i = 1; i <= arcSteps; ++i)
    {
        const float t = static_cast<float>(i) / static_cast<float>(arcSteps);
        const float angle = -halfAngleRad + (2.0f * halfAngleRad * t);

        Vector3 arcDir = rotateXZ(flatForward, angle);
        arcDir.Normalize();

        Vector3 currentPoint = origin + arcDir * m_volleyRange;

        DebugDrawAPI::drawLine(previousPoint, currentPoint, previewColor, 0, true);
        previousPoint = currentPoint;
    }
}

IMPLEMENT_SCRIPT(LyrielArrowVolley)
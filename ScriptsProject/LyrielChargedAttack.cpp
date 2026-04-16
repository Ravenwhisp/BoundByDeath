#include "pch.h"
#include "LyrielChargedAttack.h"

#include "ArrowPool.h"
#include "LyrielArrowProjectile.h"
#include "Damageable.h"
#include "PlayerState.h"
#include "PlayerRotation.h"
#include "PlayerAnimationController.h"

#include <cmath>

static const ScriptFieldInfo LyrielChargedAttackFields[] =
{
    { "Player Index", ScriptFieldType::Int, offsetof(LyrielChargedAttack, m_playerIndex) },
    { "Min Damage", ScriptFieldType::Float, offsetof(LyrielChargedAttack, m_minDamage), { 0.0f, 100.0f, 0.5f } },
    { "Max Damage", ScriptFieldType::Float, offsetof(LyrielChargedAttack, m_maxDamage), { 0.0f, 200.0f, 0.5f } },
    { "Max Charge Time", ScriptFieldType::Float, offsetof(LyrielChargedAttack, m_maxChargeTime), { 0.1f, 5.0f, 0.05f } },
    { "Attack Range", ScriptFieldType::Float, offsetof(LyrielChargedAttack, m_attackRange), { 0.0f, 50.0f, 0.1f } },
    { "Line Half Width", ScriptFieldType::Float, offsetof(LyrielChargedAttack, m_lineHalfWidth), { 0.1f, 10.0f, 0.05f } },
    { "Attack Cooldown", ScriptFieldType::Float, offsetof(LyrielChargedAttack, m_attackCooldown), { 0.0f, 10.0f, 0.05f } },
    { "Attack Lock Duration", ScriptFieldType::Float, offsetof(LyrielChargedAttack, m_attackLockDuration), { 0.0f, 2.0f, 0.01f } },
    { "Arrow Speed", ScriptFieldType::Float, offsetof(LyrielChargedAttack, m_arrowSpeed), { 0.0f, 100.0f, 0.5f } },
    { "Arrow Spawn Child Name", ScriptFieldType::String, offsetof(LyrielChargedAttack, m_arrowSpawnChildName) }
};

IMPLEMENT_SCRIPT_FIELDS(LyrielChargedAttack, LyrielChargedAttackFields)

LyrielChargedAttack::LyrielChargedAttack(GameObject* owner)
    : Script(owner)
{
}

void LyrielChargedAttack::Start()
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
        Debug::log("[LyrielChargedAttack] ArrowPool not found on owner.");
    }

    if (m_playerState == nullptr)
    {
        Debug::log("[LyrielChargedAttack] PlayerState not found on owner.");
    }

    if (m_playerRotation == nullptr)
    {
        Debug::log("[LyrielChargedAttack] PlayerRotation not found on owner.");
    }

    if (m_playerAnimationController == nullptr)
    {
        Debug::log("[LyrielChargedAttack] PlayerAnimationController not found on owner.");
    }
}

void LyrielChargedAttack::Update()
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

    if (canStartCharge() && Input::isRightTriggerJustPressed(m_playerIndex))
    {
        beginCharge();
    }

    if (m_isCharging && Input::isRightTriggerPressed(m_playerIndex))
    {
        updateCharge();
    }

    if (m_isCharging && Input::isRightTriggerReleased(m_playerIndex))
    {
        releaseChargeAndShoot();
    }
}

void LyrielChargedAttack::drawGizmo()
{
    if (!m_isCharging)
    {
        return;
    }

    Vector3 previewDirection = m_currentAimDirection;
    if (previewDirection.LengthSquared() <= 0.0001f)
    {
        previewDirection = getFallbackFacingDirection();
    }

    if (previewDirection.LengthSquared() <= 0.0001f)
    {
        return;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    const Vector3 origin = TransformAPI::getGlobalPosition(ownerTransform);

    float chargeRatio = 0.0f;
    if (m_maxChargeTime > 0.0001f)
    {
        chargeRatio = m_chargeTimer / m_maxChargeTime;
    }

    if (chargeRatio > 1.0f)
    {
        chargeRatio = 1.0f;
    }

    drawChargePreview(origin, previewDirection, chargeRatio);
}

void LyrielChargedAttack::updateCooldown()
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

void LyrielChargedAttack::updateAttackStateTimer()
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

        if (m_playerState != nullptr && m_playerState->isAttacking())
        {
            m_playerState->setState(PlayerStateType::Normal);
        }  
    }
}

bool LyrielChargedAttack::canStartCharge() const
{
    if (m_cooldownTimer > 0.0f)
    {
        return false;
    }

    if (m_playerState == nullptr)
    {
        return false;
    }

    if (m_playerState->isDowned() || m_playerState->isUsingAbility())
    {
        return false;
    }

    return true;
}

bool LyrielChargedAttack::canShoot() const
{
    if (m_playerState != nullptr && m_playerState->isDowned())
    {
        return false;
    }

    return true;
}

void LyrielChargedAttack::beginCharge()
{
    m_isCharging = true;

    setAbilityLocked(true);

    m_chargeTimer = 0.0f;
    m_currentAimDirection = Vector3::Zero;

    Vector3 aimDirection = computeAimDirection();
    if (isAimStickValid(aimDirection))
    {
        m_currentAimDirection = aimDirection;
    }
}

void LyrielChargedAttack::updateCharge()
{
    m_chargeTimer += Time::getDeltaTime();
    if (m_chargeTimer > m_maxChargeTime)
    {
        m_chargeTimer = m_maxChargeTime;
    }

    Vector3 aimDirection = computeAimDirection();
    if (isAimStickValid(aimDirection))
    {
        m_currentAimDirection = aimDirection;
    }
}

void LyrielChargedAttack::releaseChargeAndShoot()
{
    m_isCharging = false;

    if (!canShoot())
    {
        setAbilityLocked(false);
        m_chargeTimer = 0.0f;
        return;
    }

    Transform* spawnTransform = findArrowSpawnTransform();
    if (spawnTransform == nullptr)
    {
        setAbilityLocked(false);
        m_chargeTimer = 0.0f;
        return;
    }

    const Vector3 origin = TransformAPI::getGlobalPosition(spawnTransform);
    Vector3 forward = m_currentAimDirection;

    if (forward.LengthSquared() <= 0.0001f)
    {
        forward = getFallbackFacingDirection();
    }

    if (forward.LengthSquared() <= 0.0001f)
    {
        setAbilityLocked(false);
        m_chargeTimer = 0.0f;
        return;
    }

    const float damage = computeChargedDamage();

    float chargeRatio = 0.0f;
    if (m_maxChargeTime > 0.0001f)
    {
        chargeRatio = m_chargeTimer / m_maxChargeTime;
    }

    if (chargeRatio > 1.0f)
    {
        chargeRatio = 1.0f;
    }

    m_attackFacingDirection = forward;
    faceDirection(forward);

    std::vector<GameObject*> targets;
    collectEnemiesInLine(origin, forward, targets);
    applyChargedDamage(targets, damage);
    spawnChargedArrow(origin, forward, chargeRatio);

    if (m_playerState != nullptr)
    {
        m_playerState->setState(PlayerStateType::Attacking);
    }

    if (m_playerAnimationController != nullptr)
    {
        m_playerAnimationController->requestAttack();
    }

    m_attackStateTimer = m_attackLockDuration;
    m_cooldownTimer = m_attackCooldown;
    m_chargeTimer = 0.0f;

    Debug::log("[LyrielChargedAttack] Fired charged shot. Targets hit: %d Damage: %.2f", static_cast<int>(targets.size()), damage);
}

Transform* LyrielChargedAttack::findArrowSpawnTransform() const
{
    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());

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

Vector3 LyrielChargedAttack::computeAimDirection() const
{
    const Vector2 lookAxis = Input::getLookAxis(m_playerIndex);
    return Vector3(lookAxis.x, 0.0f, lookAxis.y);
}

void LyrielChargedAttack::faceDirection(const Vector3& direction)
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

Vector3 LyrielChargedAttack::getFallbackFacingDirection() const
{
    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());

    Vector3 forward = TransformAPI::getForward(ownerTransform);
    forward.y = 0.0f;

    if (forward.LengthSquared() <= 0.0001f)
    {
        return Vector3::Zero;
    }

    forward.Normalize();
    return forward;
}

float LyrielChargedAttack::computeChargedDamage() const
{
    float chargeRatio = 0.0f;

    if (m_maxChargeTime > 0.0001f)
    {
        chargeRatio = m_chargeTimer / m_maxChargeTime;
    }

    if (chargeRatio < 0.0f)
    {
        chargeRatio = 0.0f;
    }

    if (chargeRatio > 1.0f)
    {
        chargeRatio = 1.0f;
    }

    float damage = m_minDamage + (m_maxDamage - m_minDamage) * chargeRatio;

    return damage;
}

bool LyrielChargedAttack::isAimStickValid(const Vector3& direction) const
{
    Vector3 flatDirection = direction;
    flatDirection.y = 0.0f;

    return flatDirection.LengthSquared() > 0.0001f;
}

void LyrielChargedAttack::collectEnemiesInLine(const Vector3& origin, const Vector3& forward, std::vector<GameObject*>& outTargets)
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

    const float lineHalfWidthSq = m_lineHalfWidth * m_lineHalfWidth;

    for (GameObject* enemy : allEnemies)
    {
        if (enemy == nullptr)
        {
            continue;
        }

        Transform* enemyTransform = GameObjectAPI::getTransform(enemy);

        Vector3 enemyPosition = TransformAPI::getGlobalPosition(enemyTransform);
        enemyPosition.y = origin.y;

        Vector3 toEnemy = enemyPosition - origin;
        toEnemy.y = 0.0f;

        const float forwardDistance = toEnemy.Dot(flatForward);
        if (forwardDistance < 0.0f)
        {
            continue;
        }

        if (forwardDistance > m_attackRange)
        {
            continue;
        }

        Vector3 closestPoint = origin + flatForward * forwardDistance;
        Vector3 lateralOffset = enemyPosition - closestPoint;
        lateralOffset.y = 0.0f;

        if (lateralOffset.LengthSquared() <= lineHalfWidthSq)
        {
            outTargets.push_back(enemy);
        }
    }
}

void LyrielChargedAttack::applyChargedDamage(const std::vector<GameObject*>& targets, float damage)
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
            damageable->takeDamage(damage);
        }
    }
}

void LyrielChargedAttack::spawnChargedArrow(const Vector3& origin, const Vector3& forward, float chargeRatio)
{
    if (m_arrowPool == nullptr)
    {
        return;
    }

    LyrielArrowProjectile* arrow = m_arrowPool->acquireArrow();
    if (arrow == nullptr)
    {
        Debug::log("[LyrielChargedAttack] No available arrow in pool for charged shot visual.");
        return;
    }

    Vector3 flatForward = forward;
    flatForward.y = 0.0f;

    if (flatForward.LengthSquared() <= 0.0001f)
    {
        return;
    }

    flatForward.Normalize();

    float range = m_attackRange;
    // Optional: make the visual reach grow a bit with charge.
    // If you want fixed range always, just keep range = m_attackRange.
    range = m_attackRange * (0.4f + 0.6f * chargeRatio);

    float lifetime = 0.0f;
    if (m_arrowSpeed > 0.0001f)
    {
        lifetime = range / m_arrowSpeed;
    }

    arrow->launch(origin, flatForward, m_arrowSpeed, lifetime, nullptr, 0.0f);
}

void LyrielChargedAttack::drawChargePreview(const Vector3& origin, const Vector3& forward, float chargeRatio) const
{
    Vector3 flatForward = forward;
    flatForward.y = 0.0f;

    if (flatForward.LengthSquared() <= 0.0001f)
    {
        return;
    }

    flatForward.Normalize();

    if (chargeRatio < 0.0f)
    {
        chargeRatio = 0.0f;
    }

    if (chargeRatio > 1.0f)
    {
        chargeRatio = 1.0f;
    }

    const float previewRange = m_attackRange * (0.4f + 0.6f * chargeRatio);

    Vector3 right(-flatForward.z, 0.0f, flatForward.x);
    if (right.LengthSquared() <= 0.0001f)
    {
        return;
    }
    right.Normalize();

    const Vector3 previewColor(0.2f, 1.0f, 1.0f);

    const Vector3 leftStart = origin - right * m_lineHalfWidth;
    const Vector3 rightStart = origin + right * m_lineHalfWidth;

    const Vector3 leftEnd = leftStart + flatForward * previewRange;
    const Vector3 rightEnd = rightStart + flatForward * previewRange;

    DebugDrawAPI::drawLine(leftStart, leftEnd, previewColor, 0, true);
    DebugDrawAPI::drawLine(rightStart, rightEnd, previewColor, 0, true);
    DebugDrawAPI::drawLine(leftEnd, rightEnd, previewColor, 0, true);
}

void LyrielChargedAttack::setAbilityLocked(bool locked)
{
    if (m_playerState != nullptr)
    {
        m_playerState->setUsingAbility(locked);
    }
}

IMPLEMENT_SCRIPT(LyrielChargedAttack)
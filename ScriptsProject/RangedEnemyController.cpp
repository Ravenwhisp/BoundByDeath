#include "pch.h"
#include "RangedEnemyController.h"

#include <cfloat>
#include <cmath>
#include <algorithm>

static const ScriptFieldInfo RangedEnemyControllerFields[] =
{
    { "Max Health", ScriptFieldType::Float, offsetof(RangedEnemyController, m_maxHealth), { 1.0f, 10000.0f, 1.0f } },
    { "Health", ScriptFieldType::Float, offsetof(RangedEnemyController, m_health), { 0.0f, 10000.0f, 1.0f } },
    { "Detection Radius", ScriptFieldType::Float, offsetof(RangedEnemyController, m_detectionRadius), { 0.0f, 100.0f, 0.1f } },
    { "Attack Radius", ScriptFieldType::Float, offsetof(RangedEnemyController, m_attackRadius), { 0.0f, 100.0f, 0.1f } },
    { "Lose Radius", ScriptFieldType::Float, offsetof(RangedEnemyController, m_loseRadius), { 0.0f, 200.0f, 0.1f } },
    { "Move Speed", ScriptFieldType::Float, offsetof(RangedEnemyController, m_moveSpeed), { 0.0f, 20.0f, 0.1f } },

    { "Path Point Reach Distance", ScriptFieldType::Float, offsetof(RangedEnemyController, m_pathPointReachDistance), { 0.01f, 5.0f, 0.01f } },
    { "Repath Interval", ScriptFieldType::Float, offsetof(RangedEnemyController, m_repathInterval), { 0.05f, 5.0f, 0.05f } },
    { "Turn Speed Degrees", ScriptFieldType::Float, offsetof(RangedEnemyController, m_turnSpeedDegrees), { 0.0f, 1080.0f, 1.0f } },
    { "Debug Enabled", ScriptFieldType::Bool, offsetof(RangedEnemyController, m_debugEnabled) }
};

IMPLEMENT_SCRIPT_FIELDS(RangedEnemyController, RangedEnemyControllerFields)

RangedEnemyController::RangedEnemyController(GameObject* owner) : Script(owner)
{
}

void RangedEnemyController::Start()
{
    m_target = nullptr;
    m_repathTimer = 0.0f;
    m_lastTargetPosition = Vector3::Zero;
    m_isDead = false;
    m_deathTransitionRequested = false;

    m_health = (std::clamp)(m_health, 0.0f, m_maxHealth);
    m_health = std::clamp(m_health, 0.0f, m_maxHealth);

    if (m_health <= 0.0f)
    {
        m_health = m_maxHealth;
    }
    ClearPath();
}

void RangedEnemyController::Update()
{
    if (m_isDead)
    {
        ClearPath();
        return;
    }

    GameObject* newTarget = findPlayer();

    if (newTarget != m_target)
    {
        m_target = newTarget;
        ClearPath();
    }

    m_repathTimer += Time::getDeltaTime();

}

GameObject* RangedEnemyController::GetTarget() const
{
    return m_target;
}

bool RangedEnemyController::HasTarget() const
{
    return m_target != nullptr;
}

float RangedEnemyController::GetDistanceToTarget() const
{
    if (!m_target)
    {
        return FLT_MAX;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    Transform* targetTransform = GameObjectAPI::getTransform(m_target);

    if (!ownerTransform || !targetTransform)
    {
        return FLT_MAX;
    }

    Vector3 ownerPosition = TransformAPI::getPosition(ownerTransform);
    Vector3 targetPosition = TransformAPI::getPosition(targetTransform);

    Vector3 delta = targetPosition - ownerPosition;
    delta.y = 0.0f;

    return delta.Length();
}

bool RangedEnemyController::IsTargetDetected() const
{
    if (!m_target)
    {
        return false;
    }

    return GetDistanceToTarget() <= m_detectionRadius;
}

bool RangedEnemyController::IsTargetInAttackRange() const
{
    if (!m_target)
    {
        return false;
    }

    return GetDistanceToTarget() <= m_attackRadius;
}

bool RangedEnemyController::IsTargetLost() const
{
    if (!m_target)
    {
        return true;
    }

    return GetDistanceToTarget() >= m_loseRadius;
}

bool RangedEnemyController::MoveTowardsTarget()
{
    if (m_isDead)
    {
        ClearPath();
        return false;
    }

    if (!m_target)
    {
        ClearPath();
        return false;
    }

    if (!NavigationAPI::hasNavMesh())
    {
        return false;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    Transform* targetTransform = GameObjectAPI::getTransform(m_target);

    if (!ownerTransform || !targetTransform)
    {
        return false;
    }

    Vector3 ownerPosition = TransformAPI::getPosition(ownerTransform);
    Vector3 targetPosition = TransformAPI::getPosition(targetTransform);
    targetPosition.y = 0.0f;

    bool shouldRepath = !m_hasPath || (m_repathTimer >= m_repathInterval);

    if (m_hasPath)
    {
        Vector3 targetDelta = targetPosition - m_lastTargetPosition;
        targetDelta.y = 0.0f;

        if (targetDelta.LengthSquared() > 1.0f)
        {
            shouldRepath = true;
        }
    }

    if (shouldRepath)
    {
        if (!RebuildPathToTarget())
        {
            return false;
        }

        m_repathTimer = 0.0f;
        m_lastTargetPosition = targetPosition;
    }

    if (!m_hasPath || m_currentPathIndex >= m_path.size())
    {
        return false;
    }

    Vector3 currentPathPoint = m_path[m_currentPathIndex];

    Vector3 toPoint = currentPathPoint - ownerPosition;
    toPoint.y = 0.0f;

    float distanceToPoint = toPoint.Length();

    if (distanceToPoint <= m_pathPointReachDistance)
    {
        ++m_currentPathIndex;

        if (m_currentPathIndex >= m_path.size())
        {
            ClearPath();
            return false;
        }

        currentPathPoint = m_path[m_currentPathIndex];
        toPoint = currentPathPoint - ownerPosition;
        toPoint.y = 0.0f;
        distanceToPoint = toPoint.Length();

        if (distanceToPoint <= 0.0001f)
        {
            return false;
        }
    }

    toPoint.Normalize();

    const float maxStep = m_moveSpeed * Time::getDeltaTime();
    const Vector3 desiredStepTarget = ownerPosition + toPoint * maxStep;

    Vector3 nextPosition;
    if (!NavigationAPI::moveAlongSurface(ownerPosition, desiredStepTarget, nextPosition, Vector3(5.0f, 5.0f, 5.0f)))
    {
        ClearPath();
        return false;
    }

    TransformAPI::setPosition(ownerTransform, nextPosition);
    Vector3 actualStep = nextPosition - ownerPosition;
    actualStep.y = 0.0f;

    if (actualStep.LengthSquared() > 0.00001f)
    {
        Vector3 moveDir = actualStep;
        moveDir.y = 0.0f;

        if (moveDir.LengthSquared() > 0.00001f)
        {
            moveDir.Normalize();

            Vector3 currentEuler = TransformAPI::getEulerDegrees(ownerTransform);

            const float desiredYawRadians = std::atan2(moveDir.x, moveDir.z);
            float desiredYawDegrees = DirectX::XMConvertToDegrees(desiredYawRadians);

            float currentYawDegrees = currentEuler.y;
            float deltaYaw = desiredYawDegrees - currentYawDegrees;

            while (deltaYaw > 180.0f) deltaYaw -= 360.0f;
            while (deltaYaw < -180.0f) deltaYaw += 360.0f;

            const float maxStep = m_turnSpeedDegrees * Time::getDeltaTime();

            if (deltaYaw > maxStep) deltaYaw = maxStep;
            if (deltaYaw < -maxStep) deltaYaw = -maxStep;

            currentEuler.y += deltaYaw;
            TransformAPI::setRotationEuler(ownerTransform, currentEuler);
        }
    }

    if (actualStep.LengthSquared() <= 0.00001f)
    {
        ClearPath();
        return false;
    }

    return true;
}

float RangedEnemyController::GetHealth() const
{
    return m_health;
}

float RangedEnemyController::GetMaxHealth() const
{
    return m_maxHealth;
}

bool RangedEnemyController::IsAlive() const
{
    return !m_isDead;
}

void RangedEnemyController::ApplyDamage(float damage)
{
    if (m_isDead)
    {
        return;
    }

    if (damage <= 0.0f)
    {
        return;
    }

    m_health -= damage;
    if (m_health < 0.0f)
    {
        m_health = 0.0f;
    }

    if (m_debugEnabled)
    {
        Debug::log("[RangedEnemyController] Damage received: %.2f | Health: %.2f / %.2f",
            damage, m_health, m_maxHealth);
    }

    if (m_health <= 0.0f)
    {
        Kill();
    }
}

void RangedEnemyController::Kill()
{
    if (m_isDead)
    {
        return;
    }

    m_isDead = true;
    m_health = 0.0f;
    ClearPath();

    if (m_debugEnabled)
    {
        Debug::log("[RangedEnemyController] Enemy dead");
    }
}

bool RangedEnemyController::IsDead() const
{
    return m_isDead;
}

bool RangedEnemyController::TrySendDeathTrigger()
{
    if (!m_isDead)
    {
        return false;
    }

    if (m_deathTransitionRequested)
    {
        return false;
    }

    AnimationComponent* animation = AnimationAPI::getAnimationComponent(getOwner());
    if (!animation)
    {
        return false;
    }

    const bool sent = AnimationAPI::sendTrigger(animation, "Die");
    if (!sent)
    {
        return false;
    }

    m_deathTransitionRequested = true;

    if (m_debugEnabled)
    {
        Debug::log("[RangedEnemyController] Die trigger sent");
    }

    return true;
}

GameObject* RangedEnemyController::findPlayer() const
{
    const std::vector<GameObject*> players = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER);

    if (players.empty())
    {
        return nullptr;
    }

    return players.front();
}

bool RangedEnemyController::RebuildPathToTarget()
{
    if (!m_target)
    {
        return false;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    Transform* targetTransform = GameObjectAPI::getTransform(m_target);

    if (!ownerTransform || !targetTransform)
    {
        return false;
    }

    const Vector3 ownerPosition = TransformAPI::getPosition(ownerTransform);
    const Vector3 targetPosition = TransformAPI::getPosition(targetTransform);

    constexpr int MAX_PATH_POINTS = 128;
    Vector3 pathPoints[MAX_PATH_POINTS];

    const int pointCount = NavigationAPI::findStraightPath(
        ownerPosition,
        targetPosition,
        pathPoints,
        MAX_PATH_POINTS,
        Vector3(5.0f, 5.0f, 5.0f));

    if (pointCount < 2)
    {
        return false;
    }

    std::vector<Vector3> newPath(pathPoints, pathPoints + pointCount);
   
    m_path = std::move(newPath);
    m_currentPathIndex = 1;
    m_hasPath = true;

    return true;
}

void RangedEnemyController::ClearPath()
{
    m_path.clear();
    m_currentPathIndex = 0;
    m_hasPath = false;
}

IMPLEMENT_SCRIPT(RangedEnemyController)
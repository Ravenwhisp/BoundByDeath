#include "pch.h"
#include "RangedEnemyController.h"

#include "EnemyDetectionAggro.h"
#include "ArcherAttackConfig.h"

#include <cfloat>
#include <cmath>
#include <algorithm>

IMPLEMENT_SCRIPT_FIELDS(RangedEnemyController,
    SERIALIZED_FLOAT(m_moveSpeed, "Move Speed", 0.0f, 20.0f, 0.1f),
    SERIALIZED_FLOAT(m_pathPointReachDistance, "Path Point Reach Distance", 0.01f, 5.0f, 0.01f),
    SERIALIZED_FLOAT(m_repathInterval, "Repath Interval", 0.05f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_turnSpeedDegrees, "Turn Speed Degrees", 0.0f, 1080.0f, 1.0f)
)

RangedEnemyController::RangedEnemyController(GameObject* owner) : Script(owner)
{
}

void RangedEnemyController::Start()
{
    m_enemyDetectionAggro = GameObjectAPI::findScript<EnemyDetectionAggro>(getOwner());
    m_attackConfig = GameObjectAPI::findScript<ArcherAttackConfig>(getOwner());

    if (!m_enemyDetectionAggro)
    {
        Debug::warn("[RangedEnemyController] EnemyDetectionAggro not found on '%s'.", GameObjectAPI::getName(getOwner()));
    }

    if (!m_attackConfig)
    {
        Debug::warn("[RangedEnemyController] ArcherAttackConfig not found on '%s'.", GameObjectAPI::getName(getOwner()));
    }

    m_target = nullptr;
    m_repathTimer = 0.0f;
    m_lastTargetPosition = Vector3::Zero;

    clearPath();
}

void RangedEnemyController::Update()
{
    updateCurrentTarget();

    m_repathTimer += Time::getDeltaTime();
}

void RangedEnemyController::updateCurrentTarget()
{
    if (!m_enemyDetectionAggro)
    {
        m_enemyDetectionAggro = GameObjectAPI::findScript<EnemyDetectionAggro>(getOwner());
    }

    Transform* previousTarget = m_target;

    if (!m_enemyDetectionAggro)
    {
        m_target = nullptr;
    }
    else
    {
        m_target = m_enemyDetectionAggro->getCurrentTarget();
    }

    if (m_target != previousTarget)
    {
        clearPath();
    }
}

float RangedEnemyController::getDistanceToTarget() const
{
    if (!m_target)
    {
        return FLT_MAX;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    if (!ownerTransform)
    {
        return FLT_MAX;
    }

    Vector3 ownerPosition = TransformAPI::getPosition(ownerTransform);
    Vector3 targetPosition = TransformAPI::getPosition(m_target);

    Vector3 delta = targetPosition - ownerPosition;
    delta.y = 0.0f;

    return delta.Length();
}

bool RangedEnemyController::isTargetInAttackRange() const
{
    if (!m_target || !m_attackConfig)
    {
        return false;
    }

    return getDistanceToTarget() <= m_attackConfig->m_basicAttackRange;
}

bool RangedEnemyController::moveTowardsTarget()
{
    if (!m_target)
    {
        clearPath();
        return false;
    }

    if (!NavigationAPI::hasNavMesh())
    {
        return false;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    if (!ownerTransform)
    {
        return false;
    }

    Vector3 ownerPosition = TransformAPI::getPosition(ownerTransform);
    Vector3 targetPosition = TransformAPI::getPosition(m_target);
    targetPosition.y = 0.0f;

    bool shouldRepath = !m_hasPath || m_repathTimer >= m_repathInterval;

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
        if (!rebuildPathToTarget())
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
            clearPath();
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
        clearPath();
        return false;
    }

    TransformAPI::setPosition(ownerTransform, nextPosition);

    Vector3 actualStep = nextPosition - ownerPosition;
    actualStep.y = 0.0f;

    if (actualStep.LengthSquared() <= 0.00001f)
    {
        clearPath();
        return false;
    }

    rotateTowardsDirection(actualStep);

    return true;
}

bool RangedEnemyController::rebuildPathToTarget()
{
    if (!m_target)
    {
        return false;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    if (!ownerTransform)
    {
        return false;
    }

    const Vector3 ownerPosition = TransformAPI::getPosition(ownerTransform);
    const Vector3 targetPosition = TransformAPI::getPosition(m_target);

    constexpr int MAX_PATH_POINTS = 128;
    Vector3 pathPoints[MAX_PATH_POINTS];

    const int pointCount = NavigationAPI::findStraightPath(
        ownerPosition,
        targetPosition,
        pathPoints,
        MAX_PATH_POINTS,
        Vector3(5.0f, 5.0f, 5.0f)
    );

    if (pointCount < 2)
    {
        return false;
    }

    m_path = std::vector<Vector3>(pathPoints, pathPoints + pointCount);
    m_currentPathIndex = 1;
    m_hasPath = true;

    return true;
}

void RangedEnemyController::clearPath()
{
    m_path.clear();
    m_currentPathIndex = 0;
    m_hasPath = false;
}

void RangedEnemyController::rotateTowardsDirection(const Vector3& direction)
{
    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    if (!ownerTransform)
    {
        return;
    }

    Vector3 moveDir = direction;
    moveDir.y = 0.0f;

    if (moveDir.LengthSquared() <= 0.00001f)
    {
        return;
    }

    moveDir.Normalize();

    Vector3 currentEuler = TransformAPI::getEulerDegrees(ownerTransform);

    const float desiredYawRadians = std::atan2(moveDir.x, moveDir.z);
    float desiredYawDegrees = DirectX::XMConvertToDegrees(desiredYawRadians);

    float currentYawDegrees = currentEuler.y;
    float deltaYaw = desiredYawDegrees - currentYawDegrees;

    while (deltaYaw > 180.0f)
    {
        deltaYaw -= 360.0f;
    }

    while (deltaYaw < -180.0f)
    {
        deltaYaw += 360.0f;
    }

    const float maxStep = m_turnSpeedDegrees * Time::getDeltaTime();

    if (deltaYaw > maxStep)
    {
        deltaYaw = maxStep;
    }

    if (deltaYaw < -maxStep)
    {
        deltaYaw = -maxStep;
    }

    currentEuler.y += deltaYaw;
    TransformAPI::setRotationEuler(ownerTransform, currentEuler);
}

IMPLEMENT_SCRIPT(RangedEnemyController)
#include "pch.h"
#include "PlayerTargetController.h"

#include "PlayerController.h"
#include "CharacterBase.h"
#include "Damageable.h"
#include "DeathSound.h"
#include "LyrielSound.h"
#include "EnemyDamageable.h"
#include "EnemyBaseController.h"
#include "BreakableDamageable.h"
#include "BreakableObject.h"
#include "CrystalShadowMark.h"

namespace
{
    constexpr float kEnemyCacheRefreshInterval = 0.1f;
    constexpr float kNearbyCachePadding = 2.5f;
    constexpr float kCacheMovementThreshold = 1.0f;
}

IMPLEMENT_SCRIPT_FIELDS(PlayerTargetController,
    SERIALIZED_FLOAT(m_targetRange, "Target Range", 0.0f, 20.0f, 0.05f),
    SERIALIZED_FLOAT(m_targetConeAngle, "Target Cone Angle", 1.0f, 180.0f, 1.0f),
    SERIALIZED_FLOAT(m_angleWeight, "Angle Weight", 0.0f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_distanceWeight, "Distance Weight", 0.0f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_switchMargin, "Switch Margin", 0.0f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_switchCooldown, "Switch Cooldown", 0.0f, 1.0f, 0.01f)
)

PlayerTargetController::PlayerTargetController(GameObject* owner)
    : Script(owner)
{
}

void PlayerTargetController::Start()
{
    m_character = GameObjectAPI::findScript<CharacterBase>(getOwner());
    m_playerController = GameObjectAPI::findScript<PlayerController>(getOwner());

    if (m_character == nullptr)
    {
        Debug::warn("PlayerTargetController on '%s' could not find CharacterBase-derived script on the same GameObject.", GameObjectAPI::getName(getOwner()));
    }

    if (m_playerController == nullptr)
    {
        Debug::warn("PlayerTargetController on '%s' could not find PlayerController on the same GameObject.", GameObjectAPI::getName(getOwner()));
    }

    m_deathSound  = GameObjectAPI::findScript<DeathSound>(getOwner());
    m_lyrielSound = GameObjectAPI::findScript<LyrielSound>(getOwner());
}

void PlayerTargetController::Update()
{
    if (m_playerController != nullptr && m_playerController->isGameplayInputLocked())
    {
        return;
    }
    
    const float dt = Time::getDeltaTime();

    if (m_switchCooldownTimer > 0.0f)
    {
        m_switchCooldownTimer -= dt;
    }

    {
        SCRIPT_PROFILE_SCOPE("Refresh candidates");
        updateTargetsInRange();
    }

    {
        SCRIPT_PROFILE_SCOPE("Validate/default target");
        clearInvalidCurrentTarget();
        setDefaultEnemyTargetIfNeeded();
    }

    {
        SCRIPT_PROFILE_SCOPE("Aim target selection");
        updateCurrentTarget();
    }
}

void PlayerTargetController::drawGizmo()
{
    using namespace DebugDrawAPI;

    GameObject* owner = getOwner();
    Transform* ownerTransform = GameObjectAPI::getTransform(owner);
    if (ownerTransform == nullptr)
    {
        return;
    }

    const Vector3 ownerPosition = TransformAPI::getGlobalPosition(ownerTransform);

    const Vector3 green = { 0.0f, 1.0f, 0.0f };
    const Vector3 yellow = { 1.0f, 1.0f, 0.0f };

    drawCircle(ownerPosition, Vector3(0.0f, 1.0f, 0.0f), green, m_targetRange, 32.0f, 0, true);

    if (m_currentTarget != nullptr)
    {
        Transform* targetTransform = GameObjectAPI::getTransform(m_currentTarget);
        if (targetTransform != nullptr)
        {
            const Vector3 targetPosition = TransformAPI::getGlobalPosition(targetTransform);
            drawLine(ownerPosition, targetPosition, yellow, 0, true);
        }
    }
    
	const Vector3 aimDir = computeAimDirection();

    const Vector3 aimPosition = ownerPosition + aimDir * 3.0f;
    drawLine(ownerPosition, aimPosition, yellow, 0, true);

    const Vector3 posFlat = { ownerPosition.x, ownerPosition.y, ownerPosition.z };
    const float halfRad = m_targetConeAngle * 0.5f * (3.14159265f / 180.0f);
    const float range = m_targetRange;
    const Vector3 colBase = yellow;

    auto radialDir = [&](float a) -> Vector3
        {
            return Vector3(
                aimDir.x * cosf(a) + aimDir.z * sinf(a),
                0.0f,
                -aimDir.x * sinf(a) + aimDir.z * cosf(a));
        };

    // Arc outline
    DebugDrawAPI::drawLine(posFlat, posFlat + radialDir(-halfRad) * range, colBase);
    DebugDrawAPI::drawLine(posFlat, posFlat + radialDir(halfRad) * range, colBase);

}

void PlayerTargetController::updateTargetsInRange()
{
    m_targetsInRange.clear();
    m_defaultEnemyTarget = nullptr;

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    if (ownerTransform == nullptr)
    {
        return;
    }

    const Vector3 ownerPosition = TransformAPI::getGlobalPosition(ownerTransform);
    const float targetRangeSq = m_targetRange * m_targetRange;

    m_enemyCacheTimer += Time::getDeltaTime();

    Vector3 cacheMovement = ownerPosition - m_enemyCacheCenter;
    cacheMovement.y = 0.0f;
    const bool movedBeyondCacheThreshold =
        cacheMovement.LengthSquared() >= kCacheMovementThreshold * kCacheMovementThreshold;

    {
        SCRIPT_PROFILE_SCOPE("Refresh spatial caches");

        if (!m_crystalCacheValid)
        {
            refreshCrystalCache();
        }

        if (!m_nearbyCacheValid || m_enemyCacheTimer >= kEnemyCacheRefreshInterval || movedBeyondCacheThreshold)
        {
            refreshNearbyCache(ownerPosition);
        }
    }

    bool hasEnemyInRange = false;
    int bestEnemyPriority = -101;
    float bestEnemyDistanceSq = FLT_MAX;

    {
        SCRIPT_PROFILE_SCOPE("Filter cached targets");

        for (const CachedTarget& enemy : m_cachedEnemies)
        {
            if (!isCachedTargetValid(enemy))
            {
                continue;
            }

            Vector3 difference = TransformAPI::getGlobalPosition(enemy.transform) - ownerPosition;
            const float rangeDistanceSq = difference.LengthSquared();
            difference.y = 0.0f;
            const float distanceSq = difference.LengthSquared();

            if (rangeDistanceSq > targetRangeSq)
            {
                continue;
            }

            m_targetsInRange.push_back({ enemy.gameObject, enemy.enemyController, distanceSq });
            hasEnemyInRange = true;

            const int priority = enemy.enemyController != nullptr ? enemy.enemyController->getTargetPriority() : 0;
            if (m_defaultEnemyTarget == nullptr || priority > bestEnemyPriority ||
                (priority == bestEnemyPriority && distanceSq < bestEnemyDistanceSq))
            {
                m_defaultEnemyTarget = enemy.gameObject;
                bestEnemyPriority = priority;
                bestEnemyDistanceSq = distanceSq;
            }
        }

        for (const CachedTarget& breakable : m_cachedBreakables)
        {
            if (!isCachedTargetValid(breakable))
            {
                continue;
            }

            Vector3 difference = TransformAPI::getGlobalPosition(breakable.transform) - ownerPosition;
            const float rangeDistanceSq = difference.LengthSquared();
            difference.y = 0.0f;
            const float distanceSq = difference.LengthSquared();

            if (rangeDistanceSq > targetRangeSq)
            {
                continue;
            }

            if (hasEnemyInRange &&
                (breakable.breakableObject == nullptr || !breakable.breakableObject->canBeTargetedDuringCombat()))
            {
                continue;
            }

            m_targetsInRange.push_back({ breakable.gameObject, nullptr, distanceSq });
        }
    }
}

void PlayerTargetController::refreshNearbyCache(const Vector3& ownerPosition)
{
    m_cachedEnemies.clear();
    m_cachedBreakables.clear();

    const float queryRadius = m_targetRange + kNearbyCachePadding;
    std::vector<GameObject*> nearbyObjects = SceneAPI::getObjectsInCircularArea(
        Vector2(ownerPosition.x, ownerPosition.z), queryRadius, true, QuadtreeTarget::Both);

    std::sort(nearbyObjects.begin(), nearbyObjects.end());
    nearbyObjects.erase(std::unique(nearbyObjects.begin(), nearbyObjects.end()), nearbyObjects.end());
    m_cachedEnemies.reserve(nearbyObjects.size() + m_cachedCrystals.size());
    m_cachedBreakables.reserve(nearbyObjects.size());

    for (GameObject* object : nearbyObjects)
    {
        if (object == nullptr)
        {
            continue;
        }

        const Tag tag = GameObjectAPI::getTag(object);
        if (tag != Tag::ENEMY && tag != Tag::BREAKABLE)
        {
            continue;
        }

        CachedTarget target;
        target.gameObject = object;
        target.transform = GameObjectAPI::getTransform(object);
        target.damageable = GameObjectAPI::findScript<Damageable>(object);
        target.crystalShadowMark = GameObjectAPI::findScript<CrystalShadowMark>(object);

        if (target.transform == nullptr || target.damageable == nullptr)
        {
            continue;
        }

        if (tag == Tag::ENEMY)
        {
            if (target.crystalShadowMark != nullptr)
            {
                continue;
            }

            target.enemyController = GameObjectAPI::findScript<EnemyBaseController>(object);
            m_cachedEnemies.push_back(target);
        }
        else
        {
            target.breakableObject = GameObjectAPI::findScript<BreakableObject>(object);
            m_cachedBreakables.push_back(target);
        }
    }

    m_cachedEnemies.insert(m_cachedEnemies.end(), m_cachedCrystals.begin(), m_cachedCrystals.end());

    m_enemyCacheCenter = ownerPosition;
    m_enemyCacheTimer = 0.0f;
    m_nearbyCacheValid = true;
}

void PlayerTargetController::refreshCrystalCache()
{
    m_cachedCrystals.clear();

    const std::vector<GameObject*> crystalObjects = SceneAPI::findAllGameObjectsWithScript<CrystalShadowMark>();
    m_cachedCrystals.reserve(crystalObjects.size());

    for (GameObject* object : crystalObjects)
    {
        if (object == nullptr)
        {
            continue;
        }

        CachedTarget target;
        target.gameObject = object;
        target.transform = GameObjectAPI::getTransform(object);
        target.damageable = GameObjectAPI::findScript<Damageable>(object);
        target.enemyController = GameObjectAPI::findScript<EnemyBaseController>(object);
        target.crystalShadowMark = GameObjectAPI::findScript<CrystalShadowMark>(object);

        if (target.transform != nullptr && target.damageable != nullptr && target.crystalShadowMark != nullptr)
        {
            m_cachedCrystals.push_back(target);
        }
    }

    m_crystalCacheValid = true;
}

bool PlayerTargetController::isCachedTargetValid(const CachedTarget& target) const
{
    if (target.gameObject == nullptr || !SceneAPI::containsGameObject(target.gameObject))
    {
        return false;
    }

    if (!GameObjectAPI::isActiveInHierarchy(target.gameObject) || target.transform == nullptr || target.damageable == nullptr)
    {
        return false;
    }

    if (target.damageable->isDead() || target.damageable->getCurrentHp() <= 0.0f)
    {
        return false;
    }

    return target.crystalShadowMark == nullptr || !target.crystalShadowMark->isPuzzleCompleted();
}

void PlayerTargetController::updateCurrentTarget()
{
    if (!canUpdateTarget())
    {
        return;
    }

    const Vector3 aimDirection = computeAimDirection();

    if (!isAimStickValid(aimDirection))
    {
        return;
    }

    float bestScore = FLT_MAX;
    GameObject* bestTarget = findBestTarget(aimDirection, bestScore);

    if (bestTarget == nullptr)
    {
        return;
    }

    if (shouldSwitchTarget(bestTarget, aimDirection, bestScore))
    {
        setCurrentTarget(bestTarget);
    }
}

void PlayerTargetController::setCurrentTarget(GameObject* newTarget)
{
    if (m_currentTarget == newTarget)
    {
        return;
    }

    GameObject* previousTarget = m_currentTarget;
    m_currentTarget = newTarget;

    if (m_currentTarget != nullptr && previousTarget != nullptr)
    {
        m_switchCooldownTimer = m_switchCooldown;
    }

    if (m_currentTarget != nullptr)
    {
        const bool isLock = previousTarget == nullptr;

        if (m_deathSound != nullptr)
        {
            if (isLock)
            {
                m_deathSound->playLockTarget();
            }
            else
            {
                m_deathSound->playSwitchTarget();
            }
        }

        if (m_lyrielSound != nullptr)
        {
            if (isLock)
            {
                m_lyrielSound->playLockTarget();
            }
            else
            {
                m_lyrielSound->playSwitchTarget();
            }
        }

        Debug::log("[TargetSystem] Current target: %s", GameObjectAPI::getName(m_currentTarget));
    }
    else
    {
        Debug::log("[TargetSystem] No current target");
    }
}

void PlayerTargetController::clearInvalidCurrentTarget()
{
    if (m_currentTarget == nullptr)
    {
        return;
    }

    if (!isTargetInRange(m_currentTarget) || !isTargetAlive(m_currentTarget) || !isTargetable(m_currentTarget))
    {
        setCurrentTarget(nullptr);
    }
}

void PlayerTargetController::setDefaultEnemyTargetIfNeeded()
{
    if (m_currentTarget != nullptr)
    {
        return;
    }

    GameObject* defaultTarget = findDefaultEnemyTarget();
    if (defaultTarget != nullptr)
    {
        setCurrentTarget(defaultTarget);
    }
}

GameObject* PlayerTargetController::findDefaultEnemyTarget() const
{
    return m_defaultEnemyTarget;
}

bool PlayerTargetController::canUpdateTarget() const
{
    if (m_character == nullptr)
    {
        return false;
    }

    if (m_character->isDowned())
    {
        return false;
    }

    if (m_character->isUsingAbility())
    {
        return false;
    }

    return true;
}

Vector3 PlayerTargetController::computeAimDirection() const
{
    if (m_character == nullptr)
    {
        return Vector3::Zero;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    if (ownerTransform == nullptr)
    {
        return Vector3::Zero;
    }

    const Vector3 ownerPosition = TransformAPI::getGlobalPosition(ownerTransform);

    return Input::getAimDirection(ownerPosition, m_character->getPlayerIndex());
}

bool PlayerTargetController::isAimStickValid(const Vector3& direction) const
{
    Vector3 flatDirection = direction;
    flatDirection.y = 0.0f;

    return flatDirection.LengthSquared() > 0.0001f;
}

bool PlayerTargetController::tryComputeTargetScore(GameObject* target, const Vector3& aimDirection, float& outScore) const
{
    outScore = FLT_MAX;

    if (target == nullptr)
    {
        return false;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    Transform* targetTransform = GameObjectAPI::getTransform(target);

    if (ownerTransform == nullptr || targetTransform == nullptr)
    {
        return false;
    }

    Vector3 ownerPosition = TransformAPI::getGlobalPosition(ownerTransform);
    Vector3 targetPosition = TransformAPI::getGlobalPosition(targetTransform);

    ownerPosition.y = 0.0f;
    targetPosition.y = 0.0f;

    Vector3 toTarget = targetPosition - ownerPosition;

    // Reject out of range targets
    const float distanceSq = toTarget.LengthSquared();

    if (distanceSq <= 0.0001f)
    {
        return false;
    }

    const float targetRangeSq = m_targetRange * m_targetRange;

    if (distanceSq > targetRangeSq)
    {
        return false;
    }

    const float distance = sqrtf(distanceSq);

    // Normalize target and aim direction
    toTarget.Normalize();

    Vector3 flatAimDirection = aimDirection;
    flatAimDirection.y = 0.0f;

    if (flatAimDirection.LengthSquared() <= 0.0001f)
    {
        return false;
    }

    flatAimDirection.Normalize();

   // Check if target is inside the aim cone
   // Dot product tells how aligned the target is with the aim : 1.0 = perfectly aligned, 0.0 = perpendicular, -1.0 = behind.

    const float dot = flatAimDirection.Dot(toTarget);

    const float halfConeAngleRad = DirectX::XMConvertToRadians(m_targetConeAngle * 0.5f);
    const float minDot = cosf(halfConeAngleRad);

    if (dot < minDot)
    {
        return false;
    }

    // Here we compute target score, the lower the better result
    // angleScore, the closer to 0 the more aligned with the aim direction
    // distanceScore: the closer to 0 the closer to the player

    float angleScore = 1.0f - dot;

    float distanceScore = 0.0f;
    if (m_targetRange > 0.0001f)
    {
        distanceScore = distance / m_targetRange;
    }

    outScore = (angleScore * m_angleWeight) + (distanceScore * m_distanceWeight);

    return true;
}

GameObject* PlayerTargetController::findBestTarget(const Vector3& aimDirection, float& outBestScore) const
{
    GameObject* bestEnemy = nullptr;
    int bestEnemyPriority = -101;
    float bestEnemyScore = FLT_MAX;

    GameObject* bestNonEnemy = nullptr;
    float bestNonEnemyScore = FLT_MAX;

    // Priority orders enemies first; existing aim and distance scoring breaks ties.
    // Non-enemies such as breakables keep the existing geometric scoring.
    for (const TargetCandidate& candidate : m_targetsInRange)
    {
        GameObject* target = candidate.gameObject;
        float score = FLT_MAX;

        if (!tryComputeTargetScore(target, aimDirection, score))
        {
            continue;
        }

        if (GameObjectAPI::getTag(target) == Tag::ENEMY)
        {
            EnemyBaseController* controller = candidate.enemyController;
            const int priority = controller != nullptr ? controller->getTargetPriority() : 0;

            if (bestEnemy == nullptr || priority > bestEnemyPriority || (priority == bestEnemyPriority && score < bestEnemyScore))
            {
                bestEnemy = target;
                bestEnemyPriority = priority;
                bestEnemyScore = score;
            }
        }
        else if (score < bestNonEnemyScore)
        {
            bestNonEnemy = target;
            bestNonEnemyScore = score;
        }
    }

    if (bestEnemy == nullptr || (bestNonEnemy != nullptr && bestNonEnemyScore < bestEnemyScore))
    {
        outBestScore = bestNonEnemyScore;
        return bestNonEnemy;
    }

    outBestScore = bestEnemyScore;
    return bestEnemy;
}

bool PlayerTargetController::shouldSwitchTarget(GameObject* candidate, const Vector3& aimDirection, float candidateScore) const
{
    if (candidate == nullptr)
    {
        return false;
    }

    if (m_currentTarget == nullptr)
    {
        return true;
    }

    // If the candidate is already the current target, no switch needed
    if (candidate == m_currentTarget)
    {
        return false;
    }

    if (m_switchCooldownTimer > 0.0f)
    {
        return false;
    }

    // If the current target can no longer be scored then replace it
    float currentScore = FLT_MAX;
    if (!tryComputeTargetScore(m_currentTarget, aimDirection, currentScore))
    {
        return true;
    }

    if (GameObjectAPI::getTag(candidate) == Tag::ENEMY && GameObjectAPI::getTag(m_currentTarget) == Tag::ENEMY)
    {
        EnemyBaseController* candidateController = GameObjectAPI::findScript<EnemyBaseController>(candidate);
        EnemyBaseController* currentController = GameObjectAPI::findScript<EnemyBaseController>(m_currentTarget);
        const int candidatePriority = candidateController != nullptr ? candidateController->getTargetPriority() : 0;
        const int currentPriority = currentController != nullptr ? currentController->getTargetPriority() : 0;

        if (candidatePriority > currentPriority)
        {
            return true;
        }
    }

    // Only switch if the new target is clearly better than the current one.
    return candidateScore + m_switchMargin < currentScore;
}

GameObject* PlayerTargetController::findNearbyTargetInRange(float range) const
{
    if (m_targetsInRange.empty() || range <= 0.0f)
    {
        return nullptr;
    }

    GameObject* bestTarget = nullptr;
    float bestDistSq = range * range;

    for (const TargetCandidate& targetCandidate : m_targetsInRange)
    {
        GameObject* candidate = targetCandidate.gameObject;
        if (candidate == nullptr)
        {
            continue;
        }

        const float distSq = targetCandidate.distanceSq;
        if (distSq > bestDistSq)
        {
            continue;
        }

        bestDistSq = distSq;
        bestTarget = candidate;
    }

    return bestTarget;
}

bool PlayerTargetController::isTargetInRange(GameObject* target) const
{
    if (target == nullptr)
    {
        return false;
    }

    GameObject* owner = getOwner();

    Transform* ownerTransform = GameObjectAPI::getTransform(owner);
    Transform* targetTransform = GameObjectAPI::getTransform(target);

    if (ownerTransform == nullptr || targetTransform == nullptr)
    {
        return false;
    }

    const Vector3 ownerPosition = TransformAPI::getGlobalPosition(ownerTransform);
    const Vector3 targetPosition = TransformAPI::getGlobalPosition(targetTransform);

    const Vector3 distanceFromTarget = targetPosition - ownerPosition;

    return distanceFromTarget.LengthSquared() <= m_targetRange * m_targetRange;
}

bool PlayerTargetController::isTargetAlive(GameObject* target) const
{
    if (target == nullptr)
    {
        return false;
    }

    Script* damageableScript = GameObjectAPI::findScript<Damageable>(target);
    Damageable* damageable = dynamic_cast<Damageable*>(damageableScript);

    if (damageable != nullptr)
    {
        return !damageable->isDead() && damageable->getCurrentHp() > 0.0f;
    }

    return false;
}

bool PlayerTargetController::isTargetable(GameObject* target) const
{
    if (target == nullptr)
    {
        return false;
    }

    CrystalShadowMark* crystal = GameObjectAPI::findScript<CrystalShadowMark>(target);

    if (crystal != nullptr && crystal->isPuzzleCompleted())
    {
        return false;
    }

    return true;
}

bool PlayerTargetController::canTargetBreakableDuringCombat(GameObject* target) const
{
    if (target == nullptr)
    {
        return false;
    }

    BreakableObject* breakableObject = GameObjectAPI::findScript<BreakableObject>(target);

    if (breakableObject == nullptr)
    {
        return false;
    }

    return breakableObject->canBeTargetedDuringCombat();
}

IMPLEMENT_SCRIPT(PlayerTargetController)

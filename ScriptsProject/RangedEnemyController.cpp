#include "pch.h"
#include "RangedEnemyController.h"

#include "EnemyDetectionAggro.h"
#include "ArcherAttackConfig.h"

#include "Damageable.h"

RangedEnemyController::RangedEnemyController(GameObject* owner) : EnemyBaseController(owner)
{
}

void RangedEnemyController::Start()
{
    EnemyBaseController::Start();

    m_enemyDetectionAggro = GameObjectAPI::findScript<EnemyDetectionAggro>(getOwner());
    if (!m_enemyDetectionAggro)
    {
        Debug::warn("[RangedEnemyController] EnemyDetectionAggro not found on '%s'.", GameObjectAPI::getName(getOwner()));
    }

    m_currentTarget = nullptr;
    m_deathTriggerSent = false;

    resetRepathTimer();
    clearPath();
}

void RangedEnemyController::Update()
{
    const float dt = Time::getDeltaTime();

    updateCurrentTarget();
    m_somersaultCooldownTimer.update(dt);
    m_arrowBarrageCooldownTimer.update(dt);
    m_basicAttackRangeBuffTimer.update(dt);
    updateStun(dt);
}

Transform* RangedEnemyController::acquireCurrentTarget()
{
    if (!m_enemyDetectionAggro)
    {
        m_enemyDetectionAggro = GameObjectAPI::findScript<EnemyDetectionAggro>(getOwner());
    }

    if (!m_enemyDetectionAggro)
    {
        return nullptr;
    }

    return m_enemyDetectionAggro->getCurrentTarget();
}

bool RangedEnemyController::isTargetDowned(Transform* target) const
{
    if (!m_enemyDetectionAggro || !target)
    {
        return true;
    }

    return m_enemyDetectionAggro->isDowned(target);
}

const EnemyBaseAttackConfig* RangedEnemyController::getAttackConfig() const
{
    return m_attackConfig.get();
}

bool RangedEnemyController::playerInSomersaultRange() const
{
    if (!m_enemyDetectionAggro || !m_attackConfig)
    {
        return false;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    if (!ownerTransform)
    {
        return false;
    }

    Vector3 ownerPosition = TransformAPI::getGlobalPosition(ownerTransform);

    Transform* playerTransforms[] =
    {
        m_enemyDetectionAggro->getLyrielTransform(),
        m_enemyDetectionAggro->getDeathTransform()
    };

    const float triggerRangeSquared = m_attackConfig.get()->m_somersaultTriggerRange * m_attackConfig.get()->m_somersaultTriggerRange;

    for (Transform* playerTransform : playerTransforms)
    {
        if (!playerTransform)
        {
            continue;
        }

        if (m_enemyDetectionAggro->isDowned(playerTransform))
        {
            continue;
        }

        Vector3 playerPosition = TransformAPI::getGlobalPosition(playerTransform);

        Vector3 difference = playerPosition - ownerPosition;
        difference.y = 0.0f;

        if (difference.LengthSquared() <= triggerRangeSquared)
        {
            return true;
        }
    }

    return false;
}

bool RangedEnemyController::isSomersaultReady() const
{
    return m_somersaultCooldownTimer.isReady();
}

void RangedEnemyController::consumeSomersaultCooldown()
{
    if (!m_attackConfig)
    {
        return;
    }

    m_somersaultCooldownTimer.start(m_attackConfig.get()->m_somersaultCooldown);
}

void RangedEnemyController::activateSomersaultRangeBuff()
{
    if (!m_attackConfig)
    {
        return;
    }

    m_basicAttackRangeBuffTimer.start(m_attackConfig.get()->m_postSomersaultBuffDuration);
}

bool RangedEnemyController::isSomersaultRangeBuffActive() const
{
    return m_basicAttackRangeBuffTimer.isActive();
}

bool RangedEnemyController::isTargetInAttackRange() const
{
    if (!hasValidTarget())
    {
        return false;
    }

    const ArcherAttackConfig* cfg = m_attackConfig.get();
    if (!cfg)
    {
        return false;
    }

    if (isSomersaultRangeBuffActive())
    {
        return isCurrentTargetInRange(cfg->m_basicAttackRange * cfg->m_postSomersaultRangeMultiplier);
    }

    return isCurrentTargetInRange(cfg->m_basicAttackRange);
}

Vector3 RangedEnemyController::getDirectionAwayFromClosestPlayer() const
{
    if (!m_enemyDetectionAggro)
    {
        return Vector3(0.0f, 0.0f, -1.0f);
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    Vector3 ownerPosition = TransformAPI::getGlobalPosition(ownerTransform);

    Transform* lyrielTransform = m_enemyDetectionAggro->getLyrielTransform();
    Transform* deathTransform = m_enemyDetectionAggro->getDeathTransform();

    Transform* closestTransform = nullptr;
    float closestDistanceSquared = FLT_MAX;

    Transform* playerTransforms[] =
    {
        lyrielTransform,
        deathTransform
    };

    for (Transform* playerTransform : playerTransforms)
    {
        if (m_enemyDetectionAggro->isDowned(playerTransform))
        {
            continue;
        }

        Vector3 playerPosition = TransformAPI::getGlobalPosition(playerTransform);
        Vector3 difference = ownerPosition - playerPosition;
        difference.y = 0.0f;

        const float distanceSquared = difference.LengthSquared();

        if (distanceSquared < closestDistanceSquared)
        {
            closestDistanceSquared = distanceSquared;
            closestTransform = playerTransform;
        }
    }

    if (!closestTransform)
    {
        Vector3 backward = TransformAPI::getForward(ownerTransform) * -1.0f;
        backward.y = 0.0f;

        if (backward.LengthSquared() <= 0.00001f)
        {
            return Vector3(0.0f, 0.0f, -1.0f);
        }

        backward.Normalize();
        return backward;
    }

    Vector3 closestPosition = TransformAPI::getGlobalPosition(closestTransform);
    Vector3 escapeDirection = ownerPosition - closestPosition;
    escapeDirection.y = 0.0f;

    if (escapeDirection.LengthSquared() <= 0.00001f)
    {
        Vector3 backward = TransformAPI::getForward(ownerTransform) * -1.0f;
        backward.y = 0.0f;

        if (backward.LengthSquared() <= 0.00001f)
        {
            return Vector3(0.0f, 0.0f, -1.0f);
        }

        backward.Normalize();
        return backward;
    }

    escapeDirection.Normalize();
    return escapeDirection;
}

bool RangedEnemyController::isArrowBarrageReady() const
{
    return m_arrowBarrageCooldownTimer.isReady();
}

void RangedEnemyController::consumeArrowBarrageCooldown()
{
    if (!m_attackConfig)
    {
        return;
    }

    m_arrowBarrageCooldownTimer.start(m_attackConfig.get()->m_arrowBarrageCooldown);
}

bool RangedEnemyController::isTargetInArrowBarrageRange() const
{
    if (!hasValidTarget() || !m_attackConfig)
    {
        return false;
    }

    return isCurrentTargetInRange(m_attackConfig.get()->m_arrowBarrageRange);
}

IMPLEMENT_SCRIPT_FIELDS_INHERITED(RangedEnemyController, EnemyBaseController,
    SERIALIZED_ASSET_REF(m_attackConfig, "Attack Config", AssetType::DATA_CONTAINER)
)

IMPLEMENT_SCRIPT(RangedEnemyController)
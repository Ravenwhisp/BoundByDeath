#include "pch.h"
#include "EnemyController.h"

#include <algorithm>
#include <cfloat>

//static const ScriptFieldInfo EnemyControllerFields[] =
//{
//};
//
//IMPLEMENT_SCRIPT_FIELDS(EnemyController, EnemyControllerFields)

EnemyController::EnemyController(GameObject* owner)
    : Script(owner)
{
}

void EnemyController::Start()
{
    m_maxHealth = (std::max)(1.0f, m_maxHealth);
    m_health = std::clamp(m_health, 0.0f, m_maxHealth);
    m_isDead = m_health <= 0.0f;

    OnStart();
}

void EnemyController::Update()
{
    if (m_isDead)
    {
        return;
    }

    OnUpdate();
}

bool EnemyController::IsTargetDetected() const
{
    if (m_isDead || !HasTarget())
    {
        return false;
    }

    return GetDistanceToTarget() <= m_detectionRange;
}

bool EnemyController::IsTargetInAttackRange() const
{
    if (m_isDead || !HasTarget())
    {
        return false;
    }

    return GetDistanceToTarget() <= m_attackRange;
}

bool EnemyController::IsTargetLost() const
{
    if (m_isDead || !HasTarget())
    {
        return true;
    }

    return GetDistanceToTarget() >= m_loseTargetRange;
}

void EnemyController::TakeDamage(float damage)
{
    if (m_isDead)
    {
        return;
    }

    damage = (std::max)(0.0f, damage);

    if (damage <= 0.0f)
    {
        return;
    }

    m_health -= damage;

    OnDamaged(damage);

    if (m_health <= 0.0f)
    {
        m_health = 0.0f;
        m_isDead = true;

        StopMoving();
        OnDeath();
    }
}

float EnemyController::GetDistanceToTarget() const
{
    if (!getOwner() || !m_target)
    {
        return FLT_MAX;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    Transform* targetTransform = GameObjectAPI::getTransform(m_target);

    if (!ownerTransform || !targetTransform)
    {
        return FLT_MAX;
    }

    const Vector3 ownerPosition = TransformAPI::getPosition(ownerTransform);
    const Vector3 targetPosition = TransformAPI::getPosition(targetTransform);

    Vector3 delta = targetPosition - ownerPosition;
    delta.y = 0.0f;

    return delta.Length();
}

IMPLEMENT_SCRIPT(EnemyController)
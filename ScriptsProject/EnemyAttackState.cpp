#include "pch.h"
#include "EnemyAttackState.h"

#include "RangedEnemyController.h"
#include "ArcherAttackConfig.h"

#include "Damageable.h"
#include "PlayerState.h"

EnemyAttackState::EnemyAttackState(GameObject* owner)
    : StateMachineScript(owner)
{
}

void EnemyAttackState::OnStateEnter()
{
    m_archerController = GameObjectAPI::findScript<RangedEnemyController>(getOwner());
    m_attackConfig = GameObjectAPI::findScript<ArcherAttackConfig>(getOwner());

    m_stateTimer = 0.0f;
    m_cooldownTimer = 0.0f;
    m_hasAppliedDamage = false;

    if (!m_archerController)
    {
        Debug::error("[EnemyAttackState] RangedEnemyController not found.");
        return;
    }

    if (!m_attackConfig)
    {
        Debug::error("[EnemyAttackState] ArcherAttackConfig not found.");
        return;
    }

    m_archerController->clearPath();

    Debug::log("[EnemyAttackState] ENTER");
}

void EnemyAttackState::OnStateUpdate()
{
    if (!m_archerController)
    {
        return;
    }

    AnimationComponent* animation = AnimationAPI::getAnimationComponent(getOwner());
    if (!animation)
    {
        return;
    }

    if (!m_archerController->hasTarget())
    {
        AnimationAPI::sendTrigger(animation, "ToIdle");

        Debug::log("[EnemyAttackState] Idle trigger sent");

        return;
    }

    if (!m_archerController->isTargetInAttackRange())
    {
        AnimationAPI::sendTrigger(animation, "ToChase");

        Debug::log("[EnemyAttackState] Chase trigger sent");

        return;
    }

    const float dt = Time::getDeltaTime();

    if (m_cooldownTimer > 0.0f)
    {
        m_cooldownTimer -= dt;

        if (m_cooldownTimer < 0.0f)
        {
            m_cooldownTimer = 0.0f;
        }

        return;
    }

    m_stateTimer += dt;

    if (!m_hasAppliedDamage && m_stateTimer >= m_attackConfig->m_basicAttackWindupTime)
    {
        performAttack();
        m_hasAppliedDamage = true;
    }

    if (m_stateTimer >= m_attackConfig->m_basicAttackTotalDuration)
    {
        resetAttackCycle();
    }
}

void EnemyAttackState::OnStateExit()
{
    Debug::log("[EnemyAttackState] EXIT");
}

void EnemyAttackState::resetAttackCycle()
{
    m_stateTimer = 0.0f;
    m_hasAppliedDamage = false;

    if (m_attackConfig)
    {
        m_cooldownTimer = m_attackConfig->m_basicAttackCooldown;
    }
    else
    {
        m_cooldownTimer = 0.0f;
    }
}

void EnemyAttackState::performAttack()
{
    if (!m_archerController)
    {
        return;
    }

    Transform* targetTransform = m_archerController->getTarget();
    tryDamageTarget(targetTransform);
}

void EnemyAttackState::tryDamageTarget(Transform* targetTransform)
{
    if (!m_attackConfig)
    {
        return;
    }

    if (!targetTransform)
    {
        return;
    }

    GameObject* targetObject = ComponentAPI::getOwner(targetTransform);
    if (!targetObject)
    {
        return;
    }

    PlayerState* playerState = GameObjectAPI::findScript<PlayerState>(targetObject);
    if (playerState && playerState->isDowned())
    {
        return;
    }

    Damageable* damageable = GameObjectAPI::findScript<Damageable>(targetObject);
    if (!damageable)
    {
        return;
    }

    damageable->takeDamage(m_attackConfig->m_basicAttackDamage);

    Debug::log("[EnemyAttackState] Damaged '%s' for %.2f.", GameObjectAPI::getName(targetObject), m_attackConfig->m_basicAttackDamage);
}

IMPLEMENT_SCRIPT(EnemyAttackState)
#include "pch.h"
#include "ArcherAttackState.h"

#include "RangedEnemyController.h"
#include "ArcherAttackConfig.h"

#include "Damageable.h"
#include "PlayerState.h"

ArcherAttackState::ArcherAttackState(GameObject* owner)
    : StateMachineScript(owner)
{
}

void ArcherAttackState::OnStateEnter()
{
    m_archerController = GameObjectAPI::findScript<RangedEnemyController>(getOwner());
    m_attackConfig = GameObjectAPI::findScript<ArcherAttackConfig>(getOwner());
    m_animation = AnimationAPI::getAnimationComponent(getOwner());

    m_stateTimer = 0.0f;
    m_hasAppliedDamage = false;
    m_committedTarget = nullptr;

    if (!m_archerController)
    {
        Debug::error("[ArcherAttackState] RangedEnemyController not found.");
        return;
    }

    if (!m_attackConfig)
    {
        Debug::error("[ArcherAttackState] ArcherAttackConfig not found.");
        return;
    }

    if (!m_animation)
    {
        Debug::error("[ArcherAttackState] AnimationComponent not found.");
        return;
    }

    m_archerController->clearPath();
    m_archerController->resetRepathTimer();

    m_archerController->updateCurrentTarget();
    m_committedTarget = m_archerController->getCurrentTarget();

    Debug::log("[ArcherAttackState] ENTER");
}

void ArcherAttackState::OnStateUpdate()
{
    if (!m_archerController || !m_attackConfig || !m_animation)
    {
        return;
    }

    if (m_archerController->trySendDeathTrigger(m_animation))
    {
        return;
    }

    m_archerController->faceCurrentTarget();

    m_stateTimer += Time::getDeltaTime();

    if (!m_hasAppliedDamage && m_stateTimer >= m_attackConfig->m_basicAttackWindupTime)
    {
        tryDamageTarget(m_committedTarget);
        m_hasAppliedDamage = true;
    }

    if (m_stateTimer >= m_attackConfig->m_basicAttackTotalDuration)
    {
        m_archerController->updateCurrentTarget();

        if (!m_archerController->hasValidTarget())
        {
            AnimationAPI::sendTrigger(m_animation, "ToIdle");
            Debug::log("[ArcherAttackState] Attack finished, Idle trigger sent");
        }
        else
        {
            AnimationAPI::sendTrigger(m_animation, "ToChase");
            Debug::log("[ArcherAttackState] Attack finished, Chase trigger sent");
        }

        return;
    }
}

void ArcherAttackState::OnStateExit()
{
    Debug::log("[ArcherAttackState] EXIT");
}

void ArcherAttackState::tryDamageTarget(Transform* targetTransform)
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

    Debug::log("[ArcherAttackState] Damaged '%s' for %.2f.", GameObjectAPI::getName(targetObject), m_attackConfig->m_basicAttackDamage);
}

IMPLEMENT_SCRIPT(ArcherAttackState)
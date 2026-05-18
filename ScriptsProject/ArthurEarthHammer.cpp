#include "pch.h"
#include "ArthurEarthHammer.h"

#include "ArthurBossController.h"
#include "ArthurAttackConfig.h"
#include "Damageable.h"

#include <cmath>

ArthurEarthHammer::ArthurEarthHammer(GameObject* owner)
    : StateMachineScript(owner)
{
}

void ArthurEarthHammer::OnStateEnter()
{
    m_arthurController = GameObjectAPI::findScript<ArthurBossController>(getOwner());
    m_attackConfig = GameObjectAPI::findScript<ArthurAttackConfig>(getOwner());

    m_stateTimer = 0.0f;
    m_hasAppliedImpact = false;

    if (!m_arthurController)
    {
        Debug::error("[ArthurEarthHammer] ArthurBossController not found.");
        return;
    }

    if (!m_attackConfig)
    {
        Debug::error("[ArthurEarthHammer] ArthurAttackConfig not found.");
        return;
    }

    m_arthurController->clearPath();
    m_arthurController->updateCurrentTarget();
    m_arthurController->faceCurrentTarget();

    Debug::log("[ArthurEarthHammer] ENTER");
}

void ArthurEarthHammer::OnStateUpdate()
{
    if (!m_arthurController || !m_attackConfig)
    {
        return;
    }

    m_stateTimer += Time::getDeltaTime();

    if (!m_hasAppliedImpact && m_stateTimer >= m_attackConfig->m_earthHammerImpactTime)
    {
        applyImpact();
        m_hasAppliedImpact = true;
    }

    if (m_stateTimer >= m_attackConfig->m_earthHammerTotalDuration)
    {
        goToRecover();
        return;
    }
}

void ArthurEarthHammer::OnStateExit()
{
    Debug::log("[ArthurEarthHammer] EXIT");
}

void ArthurEarthHammer::applyImpact()
{
    if (!m_arthurController)
    {
        return;
    }

    // Temporary refresh while testing without final Chase logic.
    m_arthurController->updateCurrentTarget();

    Transform* focusTarget = m_arthurController->getFocusTarget();
    Transform* nonFocusTarget = m_arthurController->getNonFocusTarget();

    tryDamageTarget(focusTarget);
    tryDamageTarget(nonFocusTarget);
}

void ArthurEarthHammer::tryDamageTarget(Transform* targetTransform)
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

    if (!isTargetInsideRadius(targetTransform))
    {
        return;
    }

    Damageable* damageable = GameObjectAPI::findScript<Damageable>(targetObject);
    if (!damageable)
    {
        return;
    }

    if (damageable->isDead())
    {
        return;
    }

    damageable->takeDamage(m_attackConfig->m_earthHammerDamage);

    Debug::log("[ArthurEarthHammer] Damaged '%s' for %.2f.", GameObjectAPI::getName(targetObject), m_attackConfig->m_earthHammerDamage);

    // Still need to apply stun to the players hit in the end
}

bool ArthurEarthHammer::isTargetInsideRadius(Transform* targetTransform) const
{
    if (!m_attackConfig)
    {
        return false;
    }

    if (!targetTransform)
    {
        return false;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    if (!ownerTransform)
    {
        return false;
    }

    Vector3 ownerPosition = TransformAPI::getGlobalPosition(ownerTransform);
    Vector3 targetPosition = TransformAPI::getGlobalPosition(targetTransform);

    Vector3 difference = targetPosition - ownerPosition;
    difference.y = 0.0f;

    float distanceSquared = difference.LengthSquared();
    float radiusSquared = m_attackConfig->m_earthHammerRadius * m_attackConfig->m_earthHammerRadius;

    return distanceSquared <= radiusSquared;
}

void ArthurEarthHammer::goToRecover()
{
    if (!m_attackConfig)
    {
        return;
    }

    AnimationComponent* animation = AnimationAPI::getAnimationComponent(getOwner());
    if (!animation)
    {
        return;
    }

    if (m_arthurController)
    {
        m_arthurController->setRecoveryDuration(m_attackConfig->m_earthHammerRecoveryDuration);
    }

    Debug::log("[ArthurEarthHammer] Going to Recover.");

    AnimationAPI::sendTrigger(animation, "ToRecover");
}

IMPLEMENT_SCRIPT(ArthurEarthHammer)
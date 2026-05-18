#include "pch.h"
#include "ArthurHeavySwipe.h"

#include "ArthurBossController.h"
#include "ArthurAttackConfig.h"

#include "Damageable.h"

#include <cmath>
#include <string>

ArthurHeavySwipe::ArthurHeavySwipe(GameObject* owner)
    : StateMachineScript(owner)
{
}

void ArthurHeavySwipe::OnStateEnter()
{
    m_arthurController = GameObjectAPI::findScript<ArthurBossController>(getOwner());
    m_attackConfig = GameObjectAPI::findScript<ArthurAttackConfig>(getOwner());

    m_stateTimer = 0.0f;

    m_hit1Applied = false;
    m_hit2Applied = false;
    m_hit3Applied = false;

    if (!m_arthurController)
    {
        Debug::error("[ArthurHeavySwipe] ArthurBossController not found.");
        return;
    }

    if (!m_attackConfig)
    {
        Debug::error("[ArthurHeavySwipe] ArthurAttackConfig not found.");
        return;
    }

    m_arthurController->clearPath();
    m_arthurController->updateCurrentTarget();
    m_arthurController->faceCurrentTarget();

    Debug::log("[ArthurHeavySwipe] ENTER");
}

void ArthurHeavySwipe::OnStateUpdate()
{
    if (!m_arthurController || !m_attackConfig)
    {
        return;
    }

    m_stateTimer += Time::getDeltaTime();

    if (!m_hit1Applied && m_stateTimer >= m_attackConfig->m_heavySwipeHit1Time)
    {
        tryApplyHit(1);
        m_hit1Applied = true;
    }

    if (!m_hit2Applied && m_stateTimer >= m_attackConfig->m_heavySwipeHit2Time)
    {
        tryApplyHit(2);
        m_hit2Applied = true;
    }

    if (!m_hit3Applied && m_stateTimer >= m_attackConfig->m_heavySwipeHit3Time)
    {
        tryApplyHit(3);
        m_hit3Applied = true;
    }

    if (m_stateTimer >= m_attackConfig->m_heavySwipeTotalDuration)
    {
        goToRecover();
        return;
    }
}

void ArthurHeavySwipe::OnStateExit()
{
    Debug::log("[ArthurHeavySwipe] EXIT");
}

void ArthurHeavySwipe::tryApplyHit(int hitIndex)
{
    if (!m_arthurController)
    {
        return;
    }

    Transform* focusTarget = m_arthurController->getFocusTarget();
    Transform* nonFocusTarget = m_arthurController->getNonFocusTarget();

    tryDamageTarget(focusTarget, hitIndex);
    tryDamageTarget(nonFocusTarget, hitIndex);
}

void ArthurHeavySwipe::tryDamageTarget(Transform* targetTransform, int hitIndex)
{
    if (!m_attackConfig)
    {
        return;
    }

    if (!targetTransform)
    {
        return;
    }

    if (!isTargetInsideHitCone(targetTransform))
    {
        return;
    }

    GameObject* targetObject = ComponentAPI::getOwner(targetTransform);
    if (!targetObject)
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

    damageable->takeDamage(m_attackConfig->m_heavySwipeDamage);

    Debug::log("[ArthurHeavySwipe] Hit %d damaged '%s' for %.2f.", hitIndex, GameObjectAPI::getName(targetObject), m_attackConfig->m_heavySwipeDamage);
}

bool ArthurHeavySwipe::isTargetInsideHitCone(Transform* targetTransform) const
{
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

    Vector3 toTarget = targetPosition - ownerPosition;
    toTarget.y = 0.0f;

    float distanceSquared = toTarget.LengthSquared();
    float rangeSquared = m_attackConfig->m_heavySwipeRange * m_attackConfig->m_heavySwipeRange;

    if (distanceSquared > rangeSquared)
    {
        return false;
    }

    if (distanceSquared < 0.0001f)
    {
        return true;
    }

    toTarget.Normalize();

    Vector3 forward = TransformAPI::getForward(ownerTransform);
    forward.y = 0.0f;

    if (forward.LengthSquared() < 0.0001f)
    {
        return false;
    }

    forward.Normalize();

    float dot = forward.Dot(toTarget);

    if (dot > 1.0f)
    {
        dot = 1.0f;
    }
    else if (dot < -1.0f)
    {
        dot = -1.0f;
    }

    constexpr float degreesToRadians = 3.14159265f / 180.0f;
    float minDot = std::cos(m_attackConfig->m_heavySwipeHalfAngleDegrees * degreesToRadians);

    return dot >= minDot;
}

void ArthurHeavySwipe::goToRecover()
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
        m_arthurController->setRecoveryDuration(m_attackConfig->m_heavySwipeRecoveryDuration);
    }

    Debug::log("[ArthurHeavySwipe] Going to Recover.");

    AnimationAPI::sendTrigger(animation, "ToRecover");
}

IMPLEMENT_SCRIPT(ArthurHeavySwipe)
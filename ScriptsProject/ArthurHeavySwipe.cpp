#include "pch.h"
#include "ArthurHeavySwipe.h"

#include "ArthurBossController.h"
#include "Damageable.h"

#include <cmath>
#include <string>

IMPLEMENT_SCRIPT_FIELDS(ArthurHeavySwipe,
    SERIALIZED_FLOAT(m_damage, "Damage", 0.0f, 9999.0f, 1.0f),
    SERIALIZED_FLOAT(m_attackRange, "Attack Range", 0.0f, 20.0f, 0.1f),
    SERIALIZED_FLOAT(m_attackHalfAngleDegrees, "Attack Half Angle Degrees", 0.0f, 180.0f, 1.0f),
    SERIALIZED_FLOAT(m_totalDuration, "Total Duration", 0.1f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_hit1Time, "Hit 1 Time", 0.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_hit2Time, "Hit 2 Time", 0.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_hit3Time, "Hit 3 Time", 0.0f, 10.0f, 0.05f),
    SERIALIZED_FLOAT(m_recoveryDuration, "Recovery Duration", 0.0f, 10.0f, 0.05f),
    SERIALIZED_BOOL(m_debugEnabled, "Debug Enabled")
)

ArthurHeavySwipe::ArthurHeavySwipe(GameObject* owner)
    : StateMachineScript(owner)
{
}

void ArthurHeavySwipe::OnStateEnter()
{
    m_arthurController = GameObjectAPI::findScript<ArthurBossController>(getOwner());

    m_stateTimer = 0.0f;

    m_hit1Applied = false;
    m_hit2Applied = false;
    m_hit3Applied = false;

    if (!m_arthurController)
    {
        Debug::error("[ArthurHeavySwipe] ArthurBossController not found.");
        return;
    }

    m_arthurController->clearPath();
    m_arthurController->faceCurrentTarget();

    if (m_debugEnabled)
    {
        Debug::log("[ArthurHeavySwipe] ENTER");
    }
}

void ArthurHeavySwipe::OnStateUpdate()
{
    if (!m_arthurController)
    {
        return;
    }

    m_stateTimer += Time::getDeltaTime();

    if (!m_hit1Applied && m_stateTimer >= m_hit1Time)
    {
        tryApplyHit(1);
        m_hit1Applied = true;
    }

    if (!m_hit2Applied && m_stateTimer >= m_hit2Time)
    {
        tryApplyHit(2);
        m_hit2Applied = true;
    }

    if (!m_hit3Applied && m_stateTimer >= m_hit3Time)
    {
        tryApplyHit(3);
        m_hit3Applied = true;
    }

    if (m_stateTimer >= m_totalDuration)
    {
        goToRecover();
        return;
    }
}

void ArthurHeavySwipe::OnStateExit()
{
    if (m_debugEnabled)
    {
        Debug::log("[ArthurHeavySwipe] EXIT");
    }
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
        if (m_debugEnabled)
        {
            Debug::warn("[ArthurHeavySwipe] Hit %d found target '%s', but it has no Damageable.", hitIndex, GameObjectAPI::getName(targetObject));
        }

        return;
    }

    if (damageable->isDead())
    {
        return;
    }

    damageable->takeDamage(m_damage);

    if (m_debugEnabled)
    {
        Debug::log("[ArthurHeavySwipe] Hit %d damaged '%s' for %.2f.", hitIndex, GameObjectAPI::getName(targetObject),m_damage);
    }
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
    float rangeSquared = m_attackRange * m_attackRange;

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
    float minDot = std::cos(m_attackHalfAngleDegrees * degreesToRadians);

    return dot >= minDot;
}

void ArthurHeavySwipe::goToRecover()
{
    AnimationComponent* animation = AnimationAPI::getAnimationComponent(getOwner());
    if (!animation)
    {
        return;
    }

    if (m_arthurController)
    {
        m_arthurController->setRecoveryDuration(m_recoveryDuration);
    }

    if (m_debugEnabled)
    {
        Debug::log("[ArthurHeavySwipe] Going to Recover.");
    }

    AnimationAPI::sendTrigger(animation, "ToRecover");
}

IMPLEMENT_SCRIPT(ArthurHeavySwipe)
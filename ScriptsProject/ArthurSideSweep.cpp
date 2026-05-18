#include "pch.h"
#include "ArthurSideSweep.h"

#include "ArthurBossController.h"
#include "ArthurAttackConfig.h"
#include "Damageable.h"

#include <cmath>


ArthurSideSweep::ArthurSideSweep(GameObject* owner)
    : StateMachineScript(owner)
{
}

void ArthurSideSweep::OnStateEnter()
{
    m_arthurController = GameObjectAPI::findScript<ArthurBossController>(getOwner());
    m_attackConfig = GameObjectAPI::findScript<ArthurAttackConfig>(getOwner());

    m_stateTimer = 0.0f;
    m_hasAppliedHit = false;

    if (!m_arthurController)
    {
        Debug::error("[ArthurSideSweep] ArthurBossController not found.");
        return;
    }

    if (!m_attackConfig)
    {
        Debug::error("[ArthurSideSweep] ArthurAttackConfig not found.");
        return;
    }

    m_arthurController->clearPath();
    m_arthurController->updateCurrentTarget();

    m_sweepSide = m_arthurController->getSelectedSideSweepSide();

    Debug::log("[ArthurSideSweep] ENTER");
}

void ArthurSideSweep::OnStateUpdate()
{
    if (!m_arthurController || !m_attackConfig)
    {
        return;
    }

    m_stateTimer += Time::getDeltaTime();

    if (!m_hasAppliedHit && m_stateTimer >= m_attackConfig->m_sideSweepHitTime)
    {
        applyHit();
        m_hasAppliedHit = true;
    }

    if (m_stateTimer >= m_attackConfig->m_sideSweepTotalDuration)
    {
        goToRecover();
        return;
    }
}

void ArthurSideSweep::OnStateExit()
{
    Debug::log("[ArthurSideSweep] EXIT");
}

void ArthurSideSweep::applyHit()
{
    if (!m_arthurController)
    {
        return;
    }

    // Temporary refresh while testing without final Chase logic.
    m_arthurController->updateCurrentTarget();

    // This is here just to test, this should be called and determined just prior to entering the Side Sweep state
    m_arthurController->trySelectSideSweepSide();
    m_sweepSide = m_arthurController->getSelectedSideSweepSide();
    // The three lines abovee will be removed

    Transform* focusTarget = m_arthurController->getFocusTarget();
    Transform* nonFocusTarget = m_arthurController->getNonFocusTarget();

    tryDamageTarget(focusTarget);
    tryDamageTarget(nonFocusTarget);
}

void ArthurSideSweep::tryDamageTarget(Transform* targetTransform)
{
    if (!m_arthurController || !m_attackConfig)
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

    if (!m_arthurController->isTargetInsideSideSweepZone(targetTransform, m_sweepSide))
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

    damageable->takeDamage(m_attackConfig->m_sideSweepDamage);

    Debug::log("[ArthurSideSweep] Damaged '%s' for %.2f.", GameObjectAPI::getName(targetObject), m_attackConfig->m_sideSweepDamage);
}

void ArthurSideSweep::goToRecover()
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
        m_arthurController->setRecoveryDuration(m_attackConfig->m_sideSweepRecoveryDuration);
    }

    Debug::log("[ArthurSideSweep] Going to Recover.");

    AnimationAPI::sendTrigger(animation, "ToRecover");
}

IMPLEMENT_SCRIPT(ArthurSideSweep)
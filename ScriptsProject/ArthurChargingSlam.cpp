#include "pch.h"
#include "ArthurChargingSlam.h"

#include "ArthurBossController.h"
#include "ArthurAttackConfig.h"
#include "ArthurAttackExecutor.h"

#include "Transform2D.h"

ArthurChargingSlam::ArthurChargingSlam(GameObject* owner)
    : StateMachineScript(owner)
{
}

void ArthurChargingSlam::OnStateEnter()
{
    m_arthurController = GameObjectAPI::findScript<ArthurBossController>(getOwner());
    m_attackConfig = GameObjectAPI::findScript<ArthurAttackConfig>(getOwner());
    m_attackExecutor = GameObjectAPI::findScript<ArthurAttackExecutor>(getOwner());

    m_stateTimer = 0.0f;

    m_startPosition = Vector3(0.0f, 0.0f, 0.0f);
    m_lockedTargetPosition = Vector3(0.0f, 0.0f, 0.0f);
    m_dashDirection = Vector3(0.0f, 0.0f, 0.0f);

    m_hasStartedDash = false;
    m_hasReachedDestination = false;
    m_hasAppliedImpact = false;

    m_hasDamagedFocusDuringDash = false;
    m_hasDamagedNonFocusDuringDash = false;

    if (!m_arthurController)
    {
        Debug::error("[ArthurChargingSlam] ArthurBossController not found.");
        return;
    }

    if (!m_attackConfig)
    {
        Debug::error("[ArthurChargingSlam] ArthurAttackConfig not found.");
        return;
    }

    if (!m_attackExecutor)
    {
        Debug::error("[ArthurChargingSlam] ArthurAttackExecutor not found.");
        return;
    }

    m_arthurController->clearPath();
    m_arthurController->updateCurrentTarget();
    m_arthurController->faceCurrentTarget();

    lockTargetPosition();

    m_isFadingUI = false;
    m_uiFadeOutTimer = 0.0f;

	GameObjectAPI::setActive(m_attackConfig->m_chargingSlamUICanvasTransform->getOwner(), true);

    SliderAPI::setFillAmount(
        m_attackConfig->m_chargingSlamUIBordersSliderComponent,
        0.0f);

    SliderAPI::setFillAmount(
        m_attackConfig->m_chargingSlamUIShadowSliderComponent,
        0.0f);

    Transform2DAPI::setAlpha(
        m_attackConfig->m_chargingSlamUIBackgroundTransform2D,
        0.0f);

    Transform2DAPI::setAlpha(
        m_attackConfig->m_chargingSlamUIShadowTransform2D,
        0.0f);

    Transform2DAPI::setAlpha(
        m_attackConfig->m_chargingSlamUISpikesTransform2D,
        0.0f);

    Transform2DAPI::setAlpha(
        m_attackConfig->m_chargingSlamUIBordersTransform2D,
        1.0f);
	const float distance = Vector3::Distance(m_startPosition, m_lockedTargetPosition);
    if (m_attackConfig->m_chargingSlamUIContainerTransform2D)
    {
        m_attackConfig->m_chargingSlamUIContainerTransform2D->setBaseSize(Vector2(m_attackConfig->m_chargingSlamUIContainerTransform2D->getBaseSize().x, distance * 100.f));
    }

    Debug::log("[ArthurChargingSlam] ENTER");
}

void ArthurChargingSlam::OnStateUpdate()
{
    if (!m_arthurController || !m_attackConfig || !m_attackExecutor)
    {
        return;
    }

    m_stateTimer += Time::getDeltaTime();

    float chargingDuration = m_attackConfig->m_chargingSlamHitTime;

    if (m_arthurController->isPhase2())
    {
        chargingDuration = m_attackConfig->m_chargingSlamPhase2HitTime;
    }

    if (!m_hasStartedDash && m_stateTimer >= chargingDuration)
    {
        startDash();
    }

    if (m_hasStartedDash && !m_hasReachedDestination)
    {
        updateDash();
    }

    if (m_hasReachedDestination && !m_hasAppliedImpact)
    {
        applyImpact();
        m_hasAppliedImpact = true;
    }

    if (m_hasAppliedImpact && m_stateTimer >= m_attackConfig->m_chargingSlamTotalDuration)
    {
        goToRecover();
        return;
    }

    if (m_attackConfig->m_chargingSlamUICanvasTransform && m_attackConfig->m_chargingSlamUICanvasTransform->getOwner()->GetActive())
    {
        updateUI();
	}
}

void ArthurChargingSlam::OnStateExit()
{
    GameObjectAPI::setActive(m_attackConfig->m_chargingSlamUICanvasTransform->getOwner(), false);

    Debug::log("[ArthurChargingSlam] EXIT");
}

void ArthurChargingSlam::lockTargetPosition()
{
    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    m_startPosition = TransformAPI::getGlobalPosition(ownerTransform);

    Transform* focusTarget = m_arthurController->getFocusTarget();
    if (!focusTarget)
    {
        m_lockedTargetPosition = m_startPosition;
        m_dashDirection = Vector3(0.0f, 0.0f, 0.0f);
        return;
    }

    m_lockedTargetPosition = TransformAPI::getGlobalPosition(focusTarget);
    m_lockedTargetPosition.y = m_startPosition.y;

    m_dashDirection = m_lockedTargetPosition - m_startPosition;
    m_dashDirection.y = 0.0f;

    if (m_dashDirection.LengthSquared() < 0.0001f)
    {
        m_dashDirection = Vector3(0.0f, 0.0f, 0.0f);
        return;
    }

    m_dashDirection.Normalize();
}

void ArthurChargingSlam::startDash()
{
    m_hasStartedDash = true;

    if (m_dashDirection.LengthSquared() < 0.0001f)
    {
        m_hasReachedDestination = true;
        return;
    }

    Debug::log("[ArthurChargingSlam] Dash started.");
}

void ArthurChargingSlam::updateDash()
{
    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    if (!ownerTransform)
    {
        return;
    }

    Vector3 currentPosition = TransformAPI::getGlobalPosition(ownerTransform);

    Vector3 toDestination = m_lockedTargetPosition - currentPosition;
    toDestination.y = 0.0f;

    const float remainingDistance = toDestination.Length();

    float dashSpeed = m_attackConfig->m_chargingSlamDashSpeed;

    if (m_arthurController->isPhase2())
    {
        dashSpeed = m_attackConfig->m_chargingSlamPhase2DashSpeed;
    }

    const float stepDistance = dashSpeed * Time::getDeltaTime();

    if (remainingDistance <= 0.05f)
    {
        TransformAPI::setPosition(ownerTransform, m_lockedTargetPosition);
        m_hasReachedDestination = true;
        return;
    }

    if (stepDistance >= remainingDistance)
    {
        TransformAPI::setPosition(ownerTransform, m_lockedTargetPosition);
        m_hasReachedDestination = true;
        return;
    }

    currentPosition += m_dashDirection * stepDistance;
    TransformAPI::setPosition(ownerTransform, currentPosition);

    m_arthurController->updateCurrentTarget();

    Transform* focusTarget = m_arthurController->getFocusTarget();
    Transform* nonFocusTarget = m_arthurController->getNonFocusTarget();

    tryApplyDashDamage(focusTarget, m_hasDamagedFocusDuringDash);
    tryApplyDashDamage(nonFocusTarget, m_hasDamagedNonFocusDuringDash);
}

void ArthurChargingSlam::tryApplyDashDamage(Transform* targetTransform, bool& hasDamagedTarget)
{
    if (hasDamagedTarget)
    {
        return;
    }

    if (!m_attackExecutor || !m_attackConfig)
    {
        return;
    }

    Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
    if (!ownerTransform)
    {
        return;
    }

    Vector3 center = TransformAPI::getGlobalPosition(ownerTransform);

    const bool damaged = m_attackExecutor->tryDamageTargetInRadius(targetTransform, center, m_attackConfig->m_chargingSlamDashHitRadius, m_attackConfig->m_chargingSlamDashDamage, "ChargingSlamDash");

    if (damaged)
    {
        hasDamagedTarget = true;
    }
}

void ArthurChargingSlam::applyImpact()
{
    if (!m_attackExecutor || !m_attackConfig)
    {
        return;
    }

    m_isFadingUI = true;
    m_uiFadeOutTimer = 0.0f;

    m_attackExecutor->applyDamageAndStunInRadius(m_lockedTargetPosition, m_attackConfig->m_chargingSlamImpactRadius, m_attackConfig->m_chargingSlamFinalAreaImpactDamage, m_attackConfig->m_chargingSlamImpactStunDuration, "ChargingSlamImpact");

    Debug::log("[ArthurChargingSlam] Impact applied.");
}

void ArthurChargingSlam::goToRecover()
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
        m_arthurController->setRecoveryDuration(m_attackConfig->m_chargingSlamRecoveryDuration);
    }

    Debug::log("[ArthurChargingSlam] Going to Recover.");

    AnimationAPI::sendTrigger(animation, "ToRecover");
}
void ArthurChargingSlam::updateUI()
{
    if (!m_attackConfig)
    {
        return;
    }

    const float deltaTime = Time::getDeltaTime();

	Transform2D* container = m_attackConfig->m_chargingSlamUIContainerTransform2D;
    Transform2D* borders = m_attackConfig->m_chargingSlamUIBordersTransform2D;
    Transform2D* shadow = m_attackConfig->m_chargingSlamUIShadowTransform2D;
    Transform2D* background = m_attackConfig->m_chargingSlamUIBackgroundTransform2D;
    Transform2D* spikes = m_attackConfig->m_chargingSlamUISpikesTransform2D;

    UISlider* bordersSlider = m_attackConfig->m_chargingSlamUIBordersSliderComponent;
    UISlider* shadowSlider = m_attackConfig->m_chargingSlamUIShadowSliderComponent;

    if (!container || !borders || !shadow || !background || !spikes ||
        !bordersSlider || !shadowSlider)
    {
        return;
    }

    // CHARGING PHASE

    if (!m_hasStartedDash)
    {
        float chargeDuration = m_attackConfig->m_chargingSlamHitTime;

        if (m_arthurController->isPhase2())
        {
            chargeDuration = m_attackConfig->m_chargingSlamPhase2HitTime;
        }

        const float t = std::clamp(m_stateTimer / chargeDuration, 0.0f, 1.0f);

        const float bordersFill =
            MathAPI::evaluateEasing(
                MathAPI::EasingType::EaseOutQuad,
                t);

        SliderAPI::setFillAmount(bordersSlider, bordersFill);

        Transform2DAPI::setAlpha(shadow, t);

        SliderAPI::setFillAmount(shadowSlider, t);

        const float backgroundAlpha =
            MathAPI::evaluateEasing(
                MathAPI::EasingType::EaseInQuad,
                t);

        Transform2DAPI::setAlpha(background, backgroundAlpha);

        Transform2DAPI::setAlpha(spikes, t);

        SliderAPI::setFillOrigin(
            bordersSlider,
            FillOrigin::VerticalTop);

        SliderAPI::setFillOrigin(
            shadowSlider,
            FillOrigin::VerticalTop);

        return;
    }

    // DASH PHASE

    if (m_hasStartedDash && !m_isFadingUI)
    {
        SliderAPI::setFillOrigin(
            bordersSlider,
            FillOrigin::VerticalBottom);

        SliderAPI::setFillOrigin(
            shadowSlider,
            FillOrigin::VerticalBottom);

        Transform* ownerTransform =
            GameObjectAPI::getTransform(getOwner());

        if (!ownerTransform)
        {
            return;
        }

        Vector3 currentPosition =
            TransformAPI::getGlobalPosition(ownerTransform);

        const float totalDistance =
            Vector3::Distance(
                m_startPosition,
                m_lockedTargetPosition);

        const float remainingDistance =
            Vector3::Distance(
                currentPosition,
                m_lockedTargetPosition);

        float dashT = 1.0f;

        if (totalDistance > 0.001f)
        {
            dashT = remainingDistance / totalDistance;
        }

        dashT = std::clamp(dashT, 0.0f, 1.0f);

        SliderAPI::setFillAmount(bordersSlider, dashT);
        SliderAPI::setFillAmount(shadowSlider, dashT);
		Transform2DAPI::setPivot(container, Vector2(0.5f, dashT));
		Transform2DAPI::setAnchorMin(container, Vector2(0.5f, dashT));

        return;
    }

	// FADE OUT UI

    if (m_isFadingUI)
    {
        const float fadeDuration = 0.35f;

        m_uiFadeOutTimer += deltaTime;

        float t =
            std::clamp(
                m_uiFadeOutTimer / fadeDuration,
                0.0f,
                1.0f);

        t = MathAPI::evaluateEasing(
            MathAPI::EasingType::EaseOutQuad,
            t);

        const float alpha = 1.0f - t;

        Transform2DAPI::setAlpha(borders, alpha);
        Transform2DAPI::setAlpha(shadow, alpha);
        Transform2DAPI::setAlpha(background, alpha);
        Transform2DAPI::setAlpha(spikes, alpha);

        if (t >= 1.0f)
        {
            GameObjectAPI::setActive(m_attackConfig->m_chargingSlamUICanvasTransform->getOwner(), false);
        }
    }
}

IMPLEMENT_SCRIPT(ArthurChargingSlam)
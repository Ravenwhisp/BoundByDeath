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

    setupUI();

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
    GameObjectAPI::setActive(m_attackConfig->m_chargingSlamImpactUICanvasTransform->getOwner(), false);

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
    m_isPlayingImpactUI = true;
    m_impactUITimer = 0.0f;

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

void ArthurChargingSlam::setupUI()
{
    Transform2D* container = m_attackConfig->m_chargingSlamUIContainerTransform2D;
    Transform2D* borders = m_attackConfig->m_chargingSlamUIBordersTransform2D;
    Transform2D* shadow = m_attackConfig->m_chargingSlamUIShadowTransform2D;
    Transform2D* background = m_attackConfig->m_chargingSlamUIBackgroundTransform2D;
    Transform2D* spikes = m_attackConfig->m_chargingSlamUISpikesTransform2D;

    UISlider* bordersSlider = m_attackConfig->m_chargingSlamUIBordersSliderComponent;
    UISlider* shadowSlider = m_attackConfig->m_chargingSlamUIShadowSliderComponent;

    if (!container || !borders || !shadow || !background || !spikes || !bordersSlider || !shadowSlider)
    {
        return;
    }

	// Dash UI setup
    m_isFadingUI = false;
    m_uiFadeOutTimer = 0.0f;

    GameObjectAPI::setActive(m_attackConfig->m_chargingSlamUICanvasTransform->getOwner(), true);

    SliderAPI::setFillAmount(m_attackConfig->m_chargingSlamUIBordersSliderComponent, 0.0f);
    SliderAPI::setFillAmount(m_attackConfig->m_chargingSlamUIShadowSliderComponent, 0.0f);
    Transform2DAPI::setAlpha(m_attackConfig->m_chargingSlamUIBackgroundTransform2D, 0.0f);
    Transform2DAPI::setAlpha(m_attackConfig->m_chargingSlamUIShadowTransform2D, 0.0f);
    Transform2DAPI::setAlpha(m_attackConfig->m_chargingSlamUISpikesTransform2D, 0.0f);
    Transform2DAPI::setAlpha(m_attackConfig->m_chargingSlamUIContainerTransform2D, 1.0f);

    const float distance = Vector3::Distance(m_startPosition, m_lockedTargetPosition);
    m_attackConfig->m_chargingSlamUIContainerTransform2D->setBaseSize(Vector2(m_attackConfig->m_chargingSlamUIContainerTransform2D->getBaseSize().x, distance * 100.f));
    
	// Impact UI setup
    m_isPlayingImpactUI = false;
    m_impactUITimer = 0.0f;

    m_isFadingImpactUI = false;
    m_impactUIFadeTimer = 0.0f;

	GameObjectAPI::setActive(m_attackConfig->m_chargingSlamImpactUICanvasTransform->getOwner(), true);
    if (m_attackConfig->m_chargingSlamImpactUICanvasTransform)
    {
        TransformAPI::setPosition(m_attackConfig->m_chargingSlamImpactUICanvasTransform, Vector3(m_lockedTargetPosition.x, m_lockedTargetPosition.y, m_lockedTargetPosition.z));
		TransformAPI::setRotationEuler(m_attackConfig->m_chargingSlamImpactUICanvasTransform, Vector3(90.0f, 0.0f, atan2(m_dashDirection.z, m_dashDirection.x) * 180.0f / 3.14159265f - 90.0f));
	}

	Transform2DAPI::setAlpha(m_attackConfig->m_chargingSlamImpactUIContainerTransform2D, 0.0f);
	Transform2DAPI::setAlpha(m_attackConfig->m_chargingSlamImpactUICenterTransform2D, 0.0f);
    Transform2DAPI::setAlpha(m_attackConfig->m_chargingSlamImpactUIGlowTransform2D, 0.0f);

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

    Transform2D* impactContainer = m_attackConfig->m_chargingSlamImpactUIContainerTransform2D;
    Transform2D* impactCenter = m_attackConfig->m_chargingSlamImpactUICenterTransform2D;
    Transform2D* impactGlow = m_attackConfig->m_chargingSlamImpactUIGlowTransform2D;

    if (!container || !borders || !shadow || !background || !spikes || !bordersSlider || !shadowSlider || !impactContainer || !impactCenter || !impactGlow)
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
        Transform2DAPI::setAlpha(shadow, t);
        SliderAPI::setFillAmount(shadowSlider, t);
        Transform2DAPI::setAlpha(spikes, t);

        const float bordersFill = MathAPI::evaluateEasing(MathAPI::EasingType::EaseOutQuad, t);
        SliderAPI::setFillAmount(bordersSlider, bordersFill);

        const float backgroundAlpha = MathAPI::evaluateEasing(MathAPI::EasingType::EaseInQuad, t);
        Transform2DAPI::setAlpha(background, backgroundAlpha);

        SliderAPI::setFillOrigin(bordersSlider, FillOrigin::VerticalTop);
        SliderAPI::setFillOrigin(shadowSlider, FillOrigin::VerticalTop);
    }

    // DASH PHASE

    if (m_hasStartedDash && !m_isFadingUI)
    {
        SliderAPI::setFillOrigin(bordersSlider, FillOrigin::VerticalBottom);
        SliderAPI::setFillOrigin(shadowSlider, FillOrigin::VerticalBottom);

        Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());

        if (!ownerTransform)
        {
            return;
        }

        Vector3 currentPosition = TransformAPI::getGlobalPosition(ownerTransform);

        const float totalDistance = Vector3::Distance(m_startPosition, m_lockedTargetPosition);

        const float remainingDistance = Vector3::Distance(currentPosition, m_lockedTargetPosition);

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
    }

	// IMPACT CHARGE PHASE

    if (!m_hasAppliedImpact)
    {
        float chargeDuration = m_attackConfig->m_chargingSlamHitTime;

        if (m_arthurController->isPhase2())
        {
            chargeDuration = m_attackConfig->m_chargingSlamPhase2HitTime;
        }

        float t = std::clamp(m_stateTimer / chargeDuration, 0.0f, 1.0f);

        t = MathAPI::evaluateEasing(MathAPI::EasingType::EaseInQuad, t);

        Transform2DAPI::setAlpha(impactContainer, t);
    }

	// IMPACT PHASE

    if (m_isPlayingImpactUI)
    {
        m_impactUITimer += deltaTime;
        const float duration = 0.45f;
        float t = std::clamp(m_impactUITimer / duration, 0.0f, 1.0f);

        float centerAlpha = 1.0f - MathAPI::evaluateEasing(MathAPI::EasingType::EaseOutQuad, t);
        Transform2DAPI::setAlpha(impactCenter, centerAlpha);

        float glowAlpha = MathAPI::pingPong(t);
        glowAlpha = MathAPI::evaluateEasing(MathAPI::EasingType::EaseOutQuad, glowAlpha);

        Transform2DAPI::setAlpha(impactGlow, glowAlpha);

        if (t >= 1.0f)
        {
            m_isPlayingImpactUI = false;

            m_isFadingImpactUI = true;
            m_impactUIFadeTimer = 0.0f;
        }
    }

	// FADE OUT DASH UI

    if (m_isFadingUI)
    {
        const float fadeDuration = 0.35f;

        m_uiFadeOutTimer += deltaTime;

        float t = std::clamp(m_uiFadeOutTimer / fadeDuration, 0.0f, 1.0f);

        t = MathAPI::evaluateEasing(MathAPI::EasingType::EaseOutQuad, t);
        const float alpha = 1.0f - t;

        Transform2DAPI::setAlpha(container, alpha);

        if (t >= 1.0f)
        {
            GameObjectAPI::setActive(m_attackConfig->m_chargingSlamUICanvasTransform->getOwner(), false);
        }
    }

	// FADE OUT IMPACT UI

    if (m_isFadingImpactUI)
    {
        m_impactUIFadeTimer += deltaTime;

        const float fadeDuration = 0.35f;
        float t = std::clamp(m_impactUIFadeTimer / fadeDuration, 0.0f, 1.0f);
        t = MathAPI::evaluateEasing(MathAPI::EasingType::EaseInQuad, t);
        float alpha = 1.0f - t;
        Transform2DAPI::setAlpha(impactContainer, alpha);

        if (t >= 1.0f)
        {
            GameObjectAPI::setActive(m_attackConfig->m_chargingSlamImpactUICanvasTransform->getOwner(), false);
        }
    }
}

IMPLEMENT_SCRIPT(ArthurChargingSlam)
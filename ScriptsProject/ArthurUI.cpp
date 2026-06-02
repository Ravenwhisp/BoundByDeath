#include "pch.h"
#include "ArthurUI.h"

IMPLEMENT_SCRIPT_FIELDS(ArthurUI,
	SERIALIZED_COMPONENT_REF(m_healthBarCanvas, "Health Bar Canvas", ComponentType::TRANSFORM),
	SERIALIZED_COMPONENT_REF(m_healthBarContainer, "Health Bar Container", ComponentType::TRANSFORM2D),
	SERIALIZED_COMPONENT_REF(m_healthBarPhase2, "Health Bar Phase 2", ComponentType::TRANSFORM2D),
	SERIALIZED_FLOAT(m_healthBarDuration, "Health Bar Duration", 0.0f, 10.0f, 0.1f),

	SERIALIZED_COMPONENT_REF(m_heavySwipeUICanvas, "Heavy Swipe UI Canvas", ComponentType::TRANSFORM),
	SERIALIZED_COMPONENT_REF(m_heavySwipeUIContainer, "Heavy Swipe UI Container", ComponentType::TRANSFORM2D),
	SERIALIZED_COMPONENT_REF(m_heavySwipeUIBackground, "Heavy Swipe UI Background", ComponentType::TRANSFORM2D),
	SERIALIZED_COMPONENT_REF(m_heavySwipeUIBorder, "Heavy Swipe UI Border", ComponentType::TRANSFORM2D),
	SERIALIZED_COMPONENT_REF(m_heavySwipeUIGlow, "Heavy Swipe UI Glow", ComponentType::TRANSFORM2D),
	SERIALIZED_COMPONENT_REF(m_heavySwipeUIRightClaw, "Heavy Swipe UI Right Claw", ComponentType::TRANSFORM2D),
	SERIALIZED_COMPONENT_REF(m_heavySwipeUILeftClaw, "Heavy Swipe UI Left Claw", ComponentType::TRANSFORM2D),

	SERIALIZED_COMPONENT_REF(m_sideSweepUICanvas, "Side Sweep UI Canvas", ComponentType::TRANSFORM),
	SERIALIZED_COMPONENT_REF(m_sideSweepUIContainer, "Side Sweep UI Container", ComponentType::TRANSFORM2D),
	SERIALIZED_COMPONENT_REF(m_sideSweepUIBackground, "Side Sweep UI Background", ComponentType::TRANSFORM2D),
	SERIALIZED_COMPONENT_REF(m_sideSweepUIShadow, "Side Sweep UI Shadow", ComponentType::TRANSFORM2D)
)

ArthurUI::ArthurUI(GameObject* owner)
	: Script(owner)
{
}

void ArthurUI::Start()
{
	setupHealthUI();

	m_heavySwipeUICanvasTransform = m_heavySwipeUICanvas.getReferencedComponent();
	m_heavySwipeUIContainerTransform2D = m_heavySwipeUIContainer.getReferencedComponent();
	m_heavySwipeUIBackgroundTransform2D = m_heavySwipeUIBackground.getReferencedComponent();
	m_heavySwipeUIBorderTransform2D = m_heavySwipeUIBorder.getReferencedComponent();
	m_heavySwipeUIGlowTransform2D = m_heavySwipeUIGlow.getReferencedComponent();
	m_heavySwipeUIRightClawTransform2D = m_heavySwipeUIRightClaw.getReferencedComponent();
	m_heavySwipeUILeftClawTransform2D = m_heavySwipeUILeftClaw.getReferencedComponent();

	hideHeavySwipeUI();

	m_sideSweepUICanvasTransform = m_sideSweepUICanvas.getReferencedComponent();
	m_sideSweepUIContainerTransform2D = m_sideSweepUIContainer.getReferencedComponent();
	m_sideSweepUIBackgroundTransform2D = m_sideSweepUIBackground.getReferencedComponent();
	m_sideSweepUIShadowTransform2D = m_sideSweepUIShadow.getReferencedComponent();

	hideSideSweepUI();
}

void ArthurUI::Update()
{
	updateHealthUI();
}

void ArthurUI::setupHealthUI()
{
	m_healthBarCanvasTransform = m_healthBarCanvas.getReferencedComponent();
	m_healthBarContainerTransform2D = m_healthBarContainer.getReferencedComponent();
	m_healthBarPhase2Transform2D = m_healthBarPhase2.getReferencedComponent();

	if (!m_healthBarCanvasTransform)
	{
		Debug::warn("[ArthurUI] Health Bar Canvas reference is missing.");
		return;
	}

	GameObject* canvasOwner = m_healthBarCanvasTransform->getOwner();
	if (!canvasOwner)
	{
		Debug::warn("[ArthurUI] Health Bar Canvas owner is null.");
		return;
	}

	GameObjectAPI::setActive(canvasOwner, false);

	if (m_healthBarContainerTransform2D)
	{
		Transform2DAPI::setAlpha(m_healthBarContainerTransform2D, 0.0f);
	}

	if (m_healthBarPhase2Transform2D)
	{
		Transform2DAPI::setAlpha(m_healthBarPhase2Transform2D, 0.0f);
	}
}

void ArthurUI::updateHealthUI()
{
	if (!m_healthBarCanvasTransform || !m_healthBarContainerTransform2D)
	{
		return;
	}

	GameObject* canvasOwner = m_healthBarCanvasTransform->getOwner();
	if (!canvasOwner)
	{
		return;
	}

	if (m_healthBarTimer > 0.0f)
	{
		m_healthBarTimer -= Time::getDeltaTime();

		const float duration = m_healthBarDuration > 0.0f ? m_healthBarDuration : 0.0001f;
		const float t = std::clamp(m_healthBarTimer / duration, 0.0f, 1.0f);

		const float size = -Transform2DAPI::getBaseSize(m_healthBarContainerTransform2D).y * 0.5f;
		const float position = (m_healthBarVisible ? t : 1.0f - t) * size;
		const float alpha = m_healthBarVisible ? 1.0f - t : t;

		Transform2DAPI::setPosition(m_healthBarContainerTransform2D, Vector2(0.0f, position));
		Transform2DAPI::setAlpha(m_healthBarContainerTransform2D, alpha);
	}

	if (m_healthBarTimer <= 0.0f)
	{
		GameObjectAPI::setActive(canvasOwner, m_healthBarVisible);
		Transform2DAPI::setAlpha(m_healthBarContainerTransform2D, m_healthBarVisible ? 1.0f : 0.0f);
	}

	if (m_healthBarPhase2Transform2D && m_healthBarPhase2Timer > 0.0f)
	{
		m_healthBarPhase2Timer -= Time::getDeltaTime();

		const float duration = m_healthBarDuration > 0.0f ? m_healthBarDuration : 0.0001f;
		const float t = 1.0f - std::clamp(m_healthBarPhase2Timer / duration, 0.0f, 1.0f);

		Transform2DAPI::setAlpha(m_healthBarPhase2Transform2D, t);
	}
}

void ArthurUI::showHealthUI(bool show)
{
	if (!m_healthBarCanvasTransform)
	{
		return;
	}

	GameObject* canvasOwner = m_healthBarCanvasTransform->getOwner();
	if (!canvasOwner)
	{
		return;
	}

	m_healthBarVisible = show;
	m_healthBarTimer = m_healthBarDuration;

	GameObjectAPI::setActive(canvasOwner, true);
}

void ArthurUI::updateHealthUIPhase()
{
	if (!m_healthBarPhase2Transform2D)
	{
		return;
	}

	m_healthBarPhase2Visible = true;
	m_healthBarPhase2Timer = m_healthBarDuration;
}

void ArthurUI::setupHeavySwipeUI()
{
	if (!m_heavySwipeUICanvasTransform ||
		!m_heavySwipeUIContainerTransform2D ||
		!m_heavySwipeUIBackgroundTransform2D ||
		!m_heavySwipeUIBorderTransform2D ||
		!m_heavySwipeUIGlowTransform2D ||
		!m_heavySwipeUIRightClawTransform2D ||
		!m_heavySwipeUILeftClawTransform2D)
	{
		return;
	}

	GameObjectAPI::setActive(m_heavySwipeUICanvasTransform->getOwner(), true);

	Transform2DAPI::setAlpha(m_heavySwipeUIContainerTransform2D, 0.0f);
	Transform2DAPI::setAlpha(m_heavySwipeUIBackgroundTransform2D, 1.0f);
	Transform2DAPI::setAlpha(m_heavySwipeUIBorderTransform2D, 1.0f);
	Transform2DAPI::setAlpha(m_heavySwipeUIGlowTransform2D, 0.0f);
	Transform2DAPI::setAlpha(m_heavySwipeUIRightClawTransform2D, 0.0f);
	Transform2DAPI::setAlpha(m_heavySwipeUILeftClawTransform2D, 0.0f);
}

void ArthurUI::updateHeavySwipeUI(float stateTimer, bool isPhase2, float hit1Time, float hit2Time, float hit3Time, float hit4Time, float totalDuration, float heavySwipeRange)
{
	if (!m_heavySwipeUIContainerTransform2D || !m_heavySwipeUIBackgroundTransform2D || !m_heavySwipeUIBorderTransform2D || !m_heavySwipeUIGlowTransform2D || !m_heavySwipeUIRightClawTransform2D || !m_heavySwipeUILeftClawTransform2D)
	{
		return;
	}

	if (stateTimer < hit1Time)
	{
		const float t = std::clamp(stateTimer / hit1Time, 0.0f, 1.0f);
		Transform2DAPI::setAlpha(m_heavySwipeUIContainerTransform2D, t);
	}
	else if (stateTimer < hit2Time)
	{
		const float t = (stateTimer - hit1Time) / (hit2Time - hit1Time);
		applyHeavySwipeHitEffects(t, m_heavySwipeUIGlowTransform2D, m_heavySwipeUIBorderTransform2D, m_heavySwipeUILeftClawTransform2D, heavySwipeRange);
	}
	else if (stateTimer < hit3Time)
	{
		const float t = (stateTimer - hit2Time) / (hit3Time - hit2Time);
		applyHeavySwipeHitEffects(t, m_heavySwipeUIGlowTransform2D, m_heavySwipeUIBorderTransform2D, m_heavySwipeUIRightClawTransform2D, heavySwipeRange);
	}
	else if (isPhase2 && stateTimer < hit4Time)
	{
		const float t = (stateTimer - hit3Time) / (hit4Time - hit3Time);
		applyHeavySwipeHitEffects(t, m_heavySwipeUIGlowTransform2D, m_heavySwipeUIBorderTransform2D, m_heavySwipeUILeftClawTransform2D, heavySwipeRange);
	}
	else if (stateTimer <= totalDuration)
	{
		const float lastHitTime = isPhase2 ? hit4Time : hit3Time;
		const float t = (stateTimer - lastHitTime) / (totalDuration - lastHitTime);

		applyHeavySwipeHitEffects(t, m_heavySwipeUIGlowTransform2D, m_heavySwipeUIBorderTransform2D, isPhase2 ? m_heavySwipeUIRightClawTransform2D : m_heavySwipeUILeftClawTransform2D,heavySwipeRange);

		const float alpha = MathAPI::moveTowards(t, 1.0f, 0.3f);
		Transform2DAPI::setAlpha(m_heavySwipeUIContainerTransform2D, alpha);
	}
}

void ArthurUI::applyHeavySwipeHitEffects(float t, Transform2D* glow, Transform2D* border, Transform2D* claw, float heavySwipeRange)
{
	if (!glow || !border || !claw)
	{
		return;
	}

	const float glowAlpha = MathAPI::pingPong(t);
	Transform2DAPI::setAlpha(glow, glowAlpha);

	const float borderScale = MathAPI::evaluateEasing(MathAPI::EasingType::EaseOutQuad, std::clamp(t, 0.1f, 1.0f)) * heavySwipeRange;

	Transform2DAPI::setScale(border, Vector2(borderScale, borderScale));

	const float anchorValue = MathAPI::lerp(0.5f, 1.0f, t);

	Transform2DAPI::setAlpha(claw, t);
	Transform2DAPI::setAnchorMin(claw, Vector2(0.5f, anchorValue));
}

void ArthurUI::hideHeavySwipeUI()
{
	if (!m_heavySwipeUICanvasTransform)
	{
		return;
	}

	GameObject* owner = m_heavySwipeUICanvasTransform->getOwner();

	if (!owner)
	{
		return;
	}

	GameObjectAPI::setActive(owner, false);
}

void ArthurUI::setupSideSweepUI(int sweepSide)
{
	if (!m_sideSweepUICanvasTransform || !m_sideSweepUIContainerTransform2D || !m_sideSweepUIBackgroundTransform2D || !m_sideSweepUIShadowTransform2D)
	{
		return;
	}

	GameObjectAPI::setActive(m_sideSweepUICanvasTransform->getOwner(), true);

	if (sweepSide == -1)
	{
		TransformAPI::setRotationEuler(m_sideSweepUICanvasTransform, Vector3(90.0f, 0.0f, -90.0f));
	}
	else
	{
		TransformAPI::setRotationEuler(m_sideSweepUICanvasTransform, Vector3(90.0f, 0.0f, 90.0f));
	}

	Transform2DAPI::setAlpha(m_sideSweepUIBackgroundTransform2D, 0.0f);
	Transform2DAPI::setAlpha(m_sideSweepUIShadowTransform2D, 0.0f);
	Transform2DAPI::setAlpha(m_sideSweepUIContainerTransform2D, 1.0f);
}

void ArthurUI::updateSideSweepUI(float stateTimer, float hitTime, float totalDuration)
{
	if (!m_sideSweepUIContainerTransform2D || !m_sideSweepUIBackgroundTransform2D || !m_sideSweepUIShadowTransform2D)
	{
		return;
	}

	if (stateTimer < hitTime)
	{
		const float t = std::clamp(stateTimer / hitTime, 0.0f, 1.0f);
		Transform2DAPI::setAlpha(m_sideSweepUIBackgroundTransform2D, t);
	}
	else
	{
		Transform2DAPI::setAlpha(m_sideSweepUIBackgroundTransform2D, 1.0f);

		const float t = std::clamp((stateTimer - hitTime) / (totalDuration - hitTime), 0.0f, 1.0f);
		const float easedT = MathAPI::evaluateEasing(MathAPI::EasingType::EaseOutCubic, t);

		Transform2DAPI::setAlpha(m_sideSweepUIShadowTransform2D, easedT);
		Transform2DAPI::setAlpha(m_sideSweepUIContainerTransform2D, 1.0f - easedT);
	}
}

void ArthurUI::hideSideSweepUI()
{
	if (!m_sideSweepUICanvasTransform)
	{
		return;
	}

	GameObject* owner = m_sideSweepUICanvasTransform->getOwner();

	if (!owner)
	{
		return;
	}

	GameObjectAPI::setActive(owner, false);
}

IMPLEMENT_SCRIPT(ArthurUI)
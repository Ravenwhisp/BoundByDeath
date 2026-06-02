#pragma once

#include "ScriptAPI.h"
#include "Transform2D.h"
#include "UISlider.h"

class ArthurUI : public Script
{
	DECLARE_SCRIPT(ArthurUI)

public:
	explicit ArthurUI(GameObject* owner);

	void Start() override;
	void Update() override;

	ScriptFieldList getExposedFields() const override;

public:
	// Health
	void setupHealthUI();
	void updateHealthUI();
	void showHealthUI(bool show);
	void updateHealthUIPhase();

	// Heavy Swipe
	void setupHeavySwipeUI();
	void updateHeavySwipeUI(float stateTimer, bool isPhase2, float hit1Time, float hit2Time, float hit3Time, float hit4Time, float totalDuration, float heavySwipeRange);
	void hideHeavySwipeUI();

	// Side Sweep
	void setupSideSweepUI(int sweepSide);
	void updateSideSweepUI(float stateTimer, float hitTime, float totalDuration);
	void hideSideSweepUI();

private:
	void applyHeavySwipeHitEffects(float t, Transform2D* glow, Transform2D* border, Transform2D* claw, float heavySwipeRange);

public:
	float m_healthBarDuration = 1.0f;

private:
	// Health
	ScriptComponentRef<Transform> m_healthBarCanvas;
	ScriptComponentRef<Transform2D> m_healthBarContainer;
	ScriptComponentRef<Transform2D> m_healthBarPhase2;

	Transform* m_healthBarCanvasTransform = nullptr;
	Transform2D* m_healthBarContainerTransform2D = nullptr;
	Transform2D* m_healthBarPhase2Transform2D = nullptr;

	float m_healthBarTimer = 0.0f;
	bool m_healthBarVisible = false;

	float m_healthBarPhase2Timer = 0.0f;
	bool m_healthBarPhase2Visible = false;

	// Heavy Swipe
	ScriptComponentRef<Transform> m_heavySwipeUICanvas;
	ScriptComponentRef<Transform2D> m_heavySwipeUIContainer;
	ScriptComponentRef<Transform2D> m_heavySwipeUIBackground;
	ScriptComponentRef<Transform2D> m_heavySwipeUIBorder;
	ScriptComponentRef<Transform2D> m_heavySwipeUIGlow;
	ScriptComponentRef<Transform2D> m_heavySwipeUIRightClaw;
	ScriptComponentRef<Transform2D> m_heavySwipeUILeftClaw;

	Transform* m_heavySwipeUICanvasTransform = nullptr;
	Transform2D* m_heavySwipeUIContainerTransform2D = nullptr;
	Transform2D* m_heavySwipeUIBackgroundTransform2D = nullptr;
	Transform2D* m_heavySwipeUIBorderTransform2D = nullptr;
	Transform2D* m_heavySwipeUIGlowTransform2D = nullptr;
	Transform2D* m_heavySwipeUIRightClawTransform2D = nullptr;
	Transform2D* m_heavySwipeUILeftClawTransform2D = nullptr;

	//Side Sweep
	ScriptComponentRef<Transform> m_sideSweepUICanvas;
	ScriptComponentRef<Transform2D> m_sideSweepUIContainer;
	ScriptComponentRef<Transform2D> m_sideSweepUIBackground;
	ScriptComponentRef<Transform2D> m_sideSweepUIShadow;

	Transform* m_sideSweepUICanvasTransform = nullptr;
	Transform2D* m_sideSweepUIContainerTransform2D = nullptr;
	Transform2D* m_sideSweepUIBackgroundTransform2D = nullptr;
	Transform2D* m_sideSweepUIShadowTransform2D = nullptr;
};
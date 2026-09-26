#pragma once

#include "ScriptAPI.h"
#include "UISheet.h"
#include "UISlider.h"
#include "Transform2D.h"

class CooperativeSound;

enum class ReaperGaugeVisualState
{
    Normal,
    FullEnter,
    FullIdle
};

class ReaperGauge : public Script
{
    DECLARE_SCRIPT(ReaperGauge)

public:
    explicit ReaperGauge(GameObject* owner);

    void Start()     override;
    void Update()    override;
    void OnGameStop() override;
    void drawGizmo() override;
    void updateUI();

    FieldList getExposedFields() const override;

    void  onMarkExploited();
    void  consume();
    float getGauge()        const { return m_gauge; }
    float getGaugePercent() const;
    int   getCurrentSegments() const;
    bool  isFull()          const { return m_gauge >= m_maxGauge; }

public:
    float m_maxGauge         = 100.0f;
    int   m_numSegments      = 3;
    float m_gracePeriod      = 10.0f;
    float m_decayPerSecond   = 2.0f;

    ComponentRef<UISlider> m_reaperGaugeUI;
	ComponentRef<Transform2D> m_glowUI;
    ComponentRef<Transform2D> m_blinkAlphaUI;

    bool m_enableFullVfx = false;
    ComponentRef<Transform2D> m_fullGaugeContainer;
    ComponentRef<Transform2D> m_fullBurstUI;
    ComponentRef<UISheet> m_fullBurstSheetUI;
    ComponentRef<Transform2D> m_fullWispsUI;
    ComponentRef<UISheet> m_fullWispsSheetUI;
    ComponentRef<Transform2D> m_fullShineUI;

    AssetReference<void> m_fullLut;
    float m_fullLutFadeInDuration = 0.6f;
    float m_fullLutMinStrength = 0.7f;
    float m_fullLutMaxStrength = 1.0f;
    float m_fullLutBreathingSpeed = 1.25f;
    float m_fullEnterDuration = 0.5f;
    float m_fullPopScale = 1.06f;
    float m_fullBreathingSpeed = 2.0f;
    float m_fullBreathingIntensity = 0.18f;
    float m_fullShineInterval = 2.5f;
    float m_fullShineDuration = 0.65f;

	float m_blinkSpeed = 5.0f;
    float m_blinkAlpha = 0.25f;

private:
    float m_gauge         = 0.0f;
    float m_decayTimer    = 0.0f;
    bool  m_everExploited = false;
    bool  m_decaying      = false;
    
    UISlider* m_reaperGaugeSlider = nullptr;
	Transform2D* m_glowTransform = nullptr;
	Transform2D* m_blinkAlphaTransform = nullptr;
    Transform2D* m_fullGaugeTransform = nullptr;
    Transform2D* m_fullBurstTransform = nullptr;
    UISheet* m_fullBurstSheet = nullptr;
    Transform2D* m_fullWispsTransform = nullptr;
    UISheet* m_fullWispsSheet = nullptr;
    Transform2D* m_fullShineTransform = nullptr;

    ReaperGaugeVisualState m_visualState = ReaperGaugeVisualState::Normal;
    Vector2 m_fullGaugeBaseScale = Vector2(1.0f, 1.0f);
    float m_fullStateTimer = 0.0f;
    float m_shineTimer = 0.0f;
    float m_shineAnimTimer = 0.0f;
    bool m_shineAnimating = false;
    bool m_wasFull = false;

    bool m_lutCaptured = false;
    bool m_lutActive = false;
    bool m_previousLutEnabled = false;
    float m_previousLutStrength = 1.0f;
    float m_fullLutTimer = 0.0f;
    AssetId m_previousLutAsset;

    CooperativeSound* m_sound = nullptr;

    void beginFullVisuals(bool playAnnouncement);
    void endFullVisuals();
    void updateFullVisuals(float dt);
    void beginFullLut();
    void updateFullLut(float dt);
    void restorePreviousLut();
    void resetFullVisualComponents();
};

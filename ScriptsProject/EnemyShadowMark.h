#pragma once

#include "ScriptAPI.h"
#include "Transform2D.h"
#include "PlayerAttackType.h"

class ReaperGauge;

enum class ShadowMarkState
{
    None = 0,
    DeathOnly,
    LyrielOnly,
    Ready
};

class EnemyShadowMark : public Script
{
    DECLARE_SCRIPT(EnemyShadowMark)

public:
    explicit EnemyShadowMark(GameObject* owner);

    void Start()     override;
    void Update()    override;
    void drawGizmo() override;

    ScriptFieldList getExposedFields() const override;

    bool processAttack(PlayerAttackType attackType);
    // Temporal to compile
    void notifyDeathHit() { processAttack(PlayerAttackType::DeathBasic); }
    bool isExploitable() const { return m_state == ShadowMarkState::Ready; }
    virtual void exploit();
    ShadowMarkState getState() const { return m_state; }
	void updateUI();

private:
    ReaperGauge* findReaperGauge();

    bool isDeathAttack(PlayerAttackType attackType) const;
    bool isLyrielAttack(PlayerAttackType attackType) const;

    bool canApplyWith(PlayerAttackType attackType) const;
    bool canExploitWith(PlayerAttackType attackType) const;

    void applyDeathContribution();
    void applyLyrielContribution();

    void resetTimer();
    void resetMark();

public:
    float m_markDuration = 3.0f;
    float m_markUITargetScale = 1.0f;
    float m_markUIHeightOffset = 1.0f;
    ScriptComponentRef<Transform2D> m_canvas;
    ScriptComponentRef<Transform> m_mark_1;
    ScriptComponentRef<Transform> m_mark_2;
    ScriptComponentRef<Transform> m_mark_3;

    float m_volleyCooldownReduction = 0.20f;  // % of base cooldown removed per exploit


private:
    ShadowMarkState m_state = ShadowMarkState::None;
    float        m_timer        = 0.0f;
    ReaperGauge* m_reaperGauge  = nullptr;

    Transform2D* m_canvasTransform2D = nullptr;
	GameObject* m_mark1Object = nullptr;
    GameObject* m_mark2Object = nullptr;
	GameObject* m_mark3Object = nullptr;
	float m_startScale = 1.0f;

};


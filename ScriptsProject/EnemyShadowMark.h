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

    virtual bool processAttack(PlayerAttackType attackType);
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
    bool m_useMarkDuration = true;
    float m_markDuration = 5.0f;
    float m_markFadeDuration = 0.5f;
    ScriptComponentRef<Transform2D> m_canvas;
    ScriptComponentRef<Transform> m_mark_death;
    ScriptComponentRef<Transform> m_mark_lyriel;
    ScriptComponentRef<Transform> m_mark_both;

    float m_volleyCooldownReduction = 0.20f;  // % of base cooldown removed per exploit


private:
    ShadowMarkState m_state = ShadowMarkState::None;
    float        m_timer        = 0.0f;
    ReaperGauge* m_reaperGauge  = nullptr;

    Transform2D* m_canvasTransform2D = nullptr;
	GameObject* m_mark1Object = nullptr;
    GameObject* m_mark2Object = nullptr;
	GameObject* m_mark3Object = nullptr;

};


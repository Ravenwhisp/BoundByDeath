#pragma once

#include "ScriptAPI.h"
#include "CharacterUI.h"

class CharacterBase;
class AnimationComponent;

class AbilityBase : public Script
{

    DECLARE_SCRIPT(AbilityBase)

public:
    explicit AbilityBase(GameObject* owner);

    void Start() override;
    void Update() override;
    ScriptFieldList getExposedFields() const override;

    bool isEnabled() const { return m_isEnabled; }
    void setEnabled(bool enabled) { m_isEnabled = enabled; }

    void tryAbility();

    // Reduces the current cooldown timer by (fraction * baseCooldown).
    // Clamps to 0. Hides the CD UI if it reaches 0.
    void reduceCooldown(float fraction);

    int getSuccessfulUse() const { return m_successfulUseCount; }

protected:
	virtual void startAbility() {}

    void notifyAbilitySuccessfullyStarted();

    bool canStartAbility() const;
    virtual bool canStartSpecificAbility() const { return true; }

    virtual float getCooldown() const { return m_cooldown; }
    void updateCooldown(float dt);
	void updateAttackWindow(float dt);
	void startCooldown();
    bool isCooldownReady() const { return m_cooldownTimer <= 0.0f; }

    void setAbilityLocked(bool locked); //innecesario
    int getPlayerIndex() const; //innecesario

    virtual void beginAttackWindow(float lockDuration);
    virtual void finishAttackWindow();

    void beginAttackPresentation();

    virtual void onAttackWindowUpdate() {}
    virtual void onAttackWindowFinished() {}

    // Called once per attack, at the hit frame (m_hitStartPct of the clip) when
    // animation-driven timing is active. Override to apply damage / spawn projectiles.
    virtual void onHitFrame() {}

    // True while an attack/ability presentation window is running (both legacy and anim paths).
    bool isAttackWindowActive() const { return m_attackWindowActive; }

    // True when this ability should derive its hit timing from the animation clip.
    bool usesAnimHitTiming() const;

    Vector3 computeCameraRelativeAimDirection(float deadzoneSq = 0.0001f) const;
	Vector3 getFallbackFacingDirection() const;

    virtual void updateUI();

public:
    // --- Animation-driven timing (opt-in, tunable per-ability in the inspector) ---
    // When m_useAnimTiming is false the ability keeps the legacy fixed-timer behavior.
    bool        m_useAnimTiming = false;   // master switch
    std::string m_animStateName = "";      // anim state/clip this ability plays (empty = fall back to legacy)
    float       m_animSpeed = 1.0f;        // playback speed multiplier for the clip
    float       m_animBlendIn = 0.15f;     // crossfade time into the clip
    float       m_hitStartPct = 0.30f;     // normalized clip time [0..1] at which the hit lands
    float       m_recoverEndPct = 0.90f;   // normalized clip time [0..1] at which the window ends

protected:
    CharacterBase* m_character = nullptr;
    CharacterUI* m_characterUI = nullptr;
    int m_uiSlot = static_cast<int>(AbilityUISlot::BasicAttack);

    float m_cooldown = 0.0;
    float m_cooldownTimer = 0.0f;

    float m_attackStateTimer = 0.0f;

    int m_successfulUseCount = 0;

    bool m_isEnabled = true; //esto nunca cambia?

    // Animation-timing runtime state
    AnimationComponent* m_animComp = nullptr;
    bool  m_attackWindowActive = false;
    bool  m_hitFired = false;
    float m_attackWindowElapsed = 0.0f;
};
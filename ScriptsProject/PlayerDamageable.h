#pragma once

#include "Damageable.h"

class PlayerAnimationController;
class HeartbeatHaptic;
class DeathSound;
class LyrielSound;

class PlayerDamageable : public Damageable
{
    DECLARE_SCRIPT(PlayerDamageable)

public:
    explicit PlayerDamageable(GameObject* owner);

    void Start() override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

    float m_heartbeatThreshold = 0.5f;

protected:
    void onDamaged(float amount) override;
    void onHpDepleted() override;
    void onDeath() override;
    void onRevive() override;

private:
    PlayerAnimationController* m_playerAnimationController = nullptr;
    HeartbeatHaptic* m_haptic = nullptr;
    DeathSound*  m_deathSound  = nullptr;
    LyrielSound* m_lyrielSound = nullptr;
    PlayerRenderBufferComponent* m_playerRenderBuffer = nullptr;

    // Continuous-damage (Bound separation, DoTs) hurt handling: play the grunt once
    // on ENTERING the continuous damage, then stay silent (the heartbeat conveys the
    // ongoing tension). Re-arms only after a short gap with no continuous damage, so
    // it's independent of script Update() ordering within a frame.
    bool  m_continuousDamageActive = false;
    float m_continuousDamageTimer  = 0.0f;

    bool  m_damageHighlightActive = false;
    float m_damageHighlightTimer = 0.0f;

    void playHurtSfx();
    void playHurtVfx();
};
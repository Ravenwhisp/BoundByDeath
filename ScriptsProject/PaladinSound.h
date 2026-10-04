#pragma once

#include "EnemySound.h"

// Paladin SFX. Lives on the Paladin GameObject (needs a SOUND_SOURCE).
class PaladinSound : public EnemySound
{
    DECLARE_SCRIPT(PaladinSound)

public:
    explicit PaladinSound(GameObject* owner);

    // Warning shout: on first sighting, when a combat room starts (staggered), and every
    // so often while it has a target and is not mid-action.
    void playScream(float delay = 0.0f);

    // Charge ability (called by PaladinChargeState).
    void playChargeStart();
    void startChargeLoop();
    void stopChargeLoop();
    void playChargeImpact();   // only when the charge connects

    void stopAllLoops() override;

    void Start()  override;
    void Update() override;

    FieldList getExposedFields() const override;

    // Idle shouts while it has a target, grouped so a room of them does not shout at once.
    float m_screamMinInterval = 6.0f;
    float m_screamMaxInterval = 13.0f;

protected:
    const char* evBasicTelegraph() const override;
    const char* evBasicImpact()    const override;
    const char* evHurt()           const override;
    const char* evStun()           const override;
    const char* evDeath()          const override;
    const char* evFootstep()       const override;

private:
    void scheduleNextScream();

    uint32_t m_chargeLoopID = 0;

    class MeleeEnemyController* m_controller = nullptr;
    float m_screamTimer = 0.0f;
};

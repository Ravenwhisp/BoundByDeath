#pragma once

#include "EnemySound.h"

// Aelorin (final boss) SFX. Reuses the EnemySound base for the hurt debounce and the
// delayed-event machinery; everything else is bespoke, one method per ability.
//
// Aelorin has no locomotion (his controller never moves him, he teleports), no stun and
// no generic death state, so evBasicTelegraph, evBasicImpact, evStun, evDeath and
// evFootstep return nullptr and the death roar is fired from his death state instead.
//
// His events live in LevelBoss.bnk, and the audio is 3D: the attenuation sits on the
// Enemies_ActorMixer of that bank, so the GameObject needs a SOUND_SOURCE component for
// Wwise to have a moving emitter to measure distance against.
//
// Phase 2 grows four of the abilities, so those have a phase2 flag instead of a second
// method: the caller already knows which phase it is in.
class AelorinSound : public EnemySound
{
    DECLARE_SCRIPT(AelorinSound)

public:
    explicit AelorinSound(GameObject* owner);

    // Presence.
    void startIdleLoop();
    void stopIdleLoop();
    void playPhaseTransition();
    void playDeathRoar();

    // Seeker Sigils: cast, the fall of each sigil, and the impact. The large impact is
    // the phase 2 version.
    void playSeekerSigilsCast();
    void playSeekerSigilsFall();
    void playSeekerSigilsImpact(bool large);

    // Risen Spires.
    void playRisenSpiresWindup();
    void playRisenSpiresErupt(bool phase2);

    // Nova.
    void playNovaCharge();
    void playNovaBlast();

    // Spirit Cannon.
    void playSpiritCannonCharge();
    void playSpiritCannonFire(bool phase2);

    // Grasp of the Dead: a cast, a loop while it holds, and a release.
    void playGraspCast();
    void startGraspLoop();
    void stopGraspLoop();
    void playGraspRelease();

    // Summon.
    void playSummonCast();
    void playSummonSpawn();

    // Teleport.
    void playTeleportOut();
    void playTeleportIn();
    void playTeleportBurst();

    // Soul Cataclysm: a loop for the channel and the detonation at the end.
    void startCataclysmChannel();
    void stopCataclysmChannel();
    void playCataclysmDetonate();

    // Fury and Exhaustion, each with an entrance and a loop underneath.
    void playFuryEnter();
    void startFuryLoop();
    void stopFuryLoop();
    void playExhaustionEnter();
    void startExhaustionLoop();
    void stopExhaustionLoop();

    void stopAllLoops() override;

protected:
    void getCandidateBanks(const char* const*& outBanks, int& outCount) const override;

    const char* evBasicTelegraph() const override { return nullptr; }
    const char* evBasicImpact()    const override { return nullptr; }
    const char* evHurt()           const override;
    const char* evStun()           const override { return nullptr; }
    const char* evDeath()          const override { return nullptr; }   // his death state fires it
    const char* evFootstep()       const override { return nullptr; }   // he never walks

private:
    uint32_t m_idleLoopID = 0;
    uint32_t m_graspLoopID = 0;
    uint32_t m_cataclysmLoopID = 0;
    uint32_t m_furyLoopID = 0;
    uint32_t m_exhaustionLoopID = 0;
};

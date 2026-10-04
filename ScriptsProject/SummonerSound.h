#pragma once

#include "EnemySound.h"

// Summoner SFX. Lives on the Summoner GameObject (needs a SOUND_SOURCE).
// It has octopus legs and drags itself instead of stepping, so locomotion is a loop
// started and stopped by the movement watchdog rather than a footstep cadence.
class SummonerSound : public EnemySound
{
    DECLARE_SCRIPT(SummonerSound)

public:
    explicit SummonerSound(GameObject* owner);

    void playChargeStart();   // winds up the energy ball (1.0 s before the release)
    void playRelease();       // lets the ball go
    void playBallImpact();    // the ball bursts on its target
    void playSummoning();     // the ritual
    void playSpawn();         // the summoned enemies appear
    void playTeleportOut();   // leaves
    void playTeleportIn();    // arrives, delayed a frame so it is not heard at the old spot
    void playRecover();       // exhausted after summoning

    void stopAllLoops() override;

protected:
    void getCandidateBanks(const char* const*& outBanks, int& outCount) const override;

    void onMovementStarted() override;
    void onMovementStopped() override;

    const char* evBasicTelegraph() const override;
    const char* evBasicImpact()    const override;
    const char* evHurt()           const override;
    const char* evStun()           const override;
    const char* evDeath()          const override;
    const char* evFootstep()       const override;

private:
    bool m_crawlLoopActive = false;
};

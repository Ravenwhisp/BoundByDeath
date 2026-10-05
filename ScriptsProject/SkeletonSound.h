#pragma once

#include "EnemySound.h"

// Skeleton SFX. Lives on the Skeleton GameObject (needs a SOUND_SOURCE).
// On top of the shared set it covers the guard and the revive, which its own states drive.
class SkeletonSound : public EnemySound
{
    DECLARE_SCRIPT(SkeletonSound)

public:
    explicit SkeletonSound(GameObject* owner);

    void playGuardRaise();   // raises the guard
    void playGuardBlock();   // a hit lands on the guard and does nothing
    void playGuardBreak();   // the guard gives way
    void playRevive();       // the bones put themselves back together

protected:
    void getCandidateBanks(const char* const*& outBanks, int& outCount) const override;

    const char* evBasicTelegraph() const override;
    const char* evBasicImpact()    const override;
    const char* evHurt()           const override;
    const char* evStun()           const override;
    const char* evDeath()          const override;
    const char* evFootstep()       const override;
};

#include "pch.h"
#include "SkeletonSound.h"

namespace
{
    constexpr const char* k_swing      = "Play_Skeleton_Swing";
    constexpr const char* k_impact     = "Play_Skeleton_Impact";
    constexpr const char* k_hurt       = "Play_Skeleton_Hurt";
    constexpr const char* k_stun       = "Play_Skeleton_Stun";
    constexpr const char* k_death      = "Play_Skeleton_Death";
    constexpr const char* k_footstep   = "Play_Skeleton_Footsteps";

    constexpr const char* k_guardRaise = "Play_Skeleton_Guard_Raise";
    constexpr const char* k_guardBlock = "Play_Skeleton_Guard_Block";
    constexpr const char* k_guardBreak = "Play_Skeleton_Guard_Break";
    constexpr const char* k_revive     = "Play_Skeleton_Revive";

    const char* const k_banks[] = { "Level2.bnk", "Level1.bnk", "BossLevel.bnk" };
}

SkeletonSound::SkeletonSound(GameObject* owner)
    : EnemySound(owner)
{
}

void SkeletonSound::playGuardRaise() { postEvent(k_guardRaise); }
void SkeletonSound::playGuardBlock() { postEvent(k_guardBlock); }
void SkeletonSound::playGuardBreak() { postEvent(k_guardBreak); }
void SkeletonSound::playRevive()     { postEvent(k_revive); }

void SkeletonSound::getCandidateBanks(const char* const*& outBanks, int& outCount) const
{
    outBanks = k_banks;
    outCount = static_cast<int>(sizeof(k_banks) / sizeof(k_banks[0]));
}

const char* SkeletonSound::evBasicTelegraph() const { return k_swing; }
const char* SkeletonSound::evBasicImpact()    const { return k_impact; }
const char* SkeletonSound::evHurt()           const { return k_hurt; }
const char* SkeletonSound::evStun()           const { return k_stun; }
const char* SkeletonSound::evDeath()          const { return k_death; }
const char* SkeletonSound::evFootstep()       const { return k_footstep; }

IMPLEMENT_SCRIPT(SkeletonSound)

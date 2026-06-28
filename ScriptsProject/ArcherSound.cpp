#include "pch.h"
#include "ArcherSound.h"

namespace
{
    constexpr const char* k_basicRelease = "Play_Archer_Basic_Release";
    constexpr const char* k_basicImpact  = "Play_Archer_Basic_Impact";
    constexpr const char* k_hurt         = "Play_Archer_Hurt";
    constexpr const char* k_stun         = "Play_Archer_Stun";
    constexpr const char* k_death        = "Play_Archer_Death";
    constexpr const char* k_footstep     = "Play_ArcherFootsteps";

    // Volley: 4 shots staggered. The random container picks a different variation per
    // shot, so the reused basic-shot SFX doesn't sound cloned.
    constexpr int   k_barrageShots   = 4;
    constexpr float k_barrageStagger = 0.08f; // 80 ms between shots → ~240 ms volley
}

ArcherSound::ArcherSound(GameObject* owner)
    : EnemySound(owner)
{
}

const char* ArcherSound::evBasicTelegraph() const { return k_basicRelease; }
const char* ArcherSound::evBasicImpact()    const { return k_basicImpact; }
const char* ArcherSound::evHurt()           const { return k_hurt; }
const char* ArcherSound::evStun()           const { return k_stun; }
const char* ArcherSound::evDeath()          const { return k_death; }
const char* ArcherSound::evFootstep()       const { return k_footstep; }

void ArcherSound::playBarrageReleaseVolley()
{
    for (int i = 0; i < k_barrageShots; ++i)
    {
        postEventDelayed(k_basicRelease, i * k_barrageStagger);
    }
}

void ArcherSound::playBarrageImpactVolley()
{
    for (int i = 0; i < k_barrageShots; ++i)
    {
        postEventDelayed(k_basicImpact, i * k_barrageStagger);
    }
}

IMPLEMENT_SCRIPT(ArcherSound)

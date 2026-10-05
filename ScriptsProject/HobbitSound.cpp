#include "pch.h"
#include "HobbitSound.h"

namespace
{
    // Borrowed from the Archer: body, voice and feet.
    constexpr const char* k_footstep = "Play_ArcherFootsteps";
    constexpr const char* k_hurt     = "Play_Archer_Hurt";
    constexpr const char* k_stun     = "Play_Archer_Stun";
    constexpr const char* k_death    = "Play_Archer_Death";

    // Borrowed from the Paladin: the weapon.
    constexpr const char* k_swing    = "Play_Paladin_Basic_Swing";
    constexpr const char* k_impact   = "Play_Paladin_Basic_Impact";

    const char* const k_banks[] = { "Level1.bnk", "Level2.bnk", "BossLevel.bnk" };
}

HobbitSound::HobbitSound(GameObject* owner)
    : EnemySound(owner)
{
}

void HobbitSound::getCandidateBanks(const char* const*& outBanks, int& outCount) const
{
    outBanks = k_banks;
    outCount = static_cast<int>(sizeof(k_banks) / sizeof(k_banks[0]));
}

const char* HobbitSound::evBasicTelegraph() const { return k_swing; }
const char* HobbitSound::evBasicImpact()    const { return k_impact; }
const char* HobbitSound::evHurt()           const { return k_hurt; }
const char* HobbitSound::evStun()           const { return k_stun; }
const char* HobbitSound::evDeath()          const { return k_death; }
const char* HobbitSound::evFootstep()       const { return k_footstep; }

IMPLEMENT_SCRIPT(HobbitSound)

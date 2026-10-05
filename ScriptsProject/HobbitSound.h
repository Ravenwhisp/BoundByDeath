#pragma once

#include "EnemySound.h"

// Hobbit SFX (the "Enemy Dwarf" prefab). It drives the spider controller but is not a
// spider, so instead of its own set it borrows what already exists: the Archer's body
// sounds and the Paladin's weapon, which are both in the Level1 bank.
class HobbitSound : public EnemySound
{
    DECLARE_SCRIPT(HobbitSound)

public:
    explicit HobbitSound(GameObject* owner);

protected:
    void getCandidateBanks(const char* const*& outBanks, int& outCount) const override;

    const char* evBasicTelegraph() const override;
    const char* evBasicImpact()    const override;
    const char* evHurt()           const override;
    const char* evStun()           const override;
    const char* evDeath()          const override;
    const char* evFootstep()       const override;
};

#include "pch.h"
#include "SummonerSound.h"

namespace
{
    constexpr const char* k_charge      = "Play_Summoner_Charge";
    constexpr const char* k_release     = "Play_Summoner_Release";
    constexpr const char* k_impact      = "Play_Summoner_Impact";
    constexpr const char* k_summoning   = "Play_Summoner_Summoning";
    constexpr const char* k_spawn       = "Play_Summoner_Spawn";
    constexpr const char* k_teleportOut = "Play_Summoner_Teleport_Out";
    constexpr const char* k_teleportIn  = "Play_Summoner_Teleport_In";
    constexpr const char* k_recover     = "Play_Summoner_Recover";
    constexpr const char* k_hurt        = "Play_Summoner_Hurt";
    constexpr const char* k_stun        = "Play_Summoner_Stun";
    constexpr const char* k_death       = "Play_Summoner_Death";

    constexpr const char* k_crawlStart  = "Play_Summoner_Crawl";
    constexpr const char* k_crawlStop   = "Stop_Summoner_Crawl";

    const char* const k_banks[] = { "Level2.bnk", "Level1.bnk", "BossLevel.bnk" };

    // The teleport moves the object and ends in the same frame, so the arrival has to wait
    // one tick or Wwise would still place it at the departure point.
    constexpr float k_teleportInDelay = 0.05f;
}

SummonerSound::SummonerSound(GameObject* owner)
    : EnemySound(owner)
{
}

void SummonerSound::playChargeStart() { postEvent(k_charge); }
void SummonerSound::playRelease()     { postEvent(k_release); }
void SummonerSound::playBallImpact()  { postEvent(k_impact); }
void SummonerSound::playSummoning()   { postEvent(k_summoning); }
void SummonerSound::playSpawn()       { postEvent(k_spawn); }
void SummonerSound::playTeleportOut() { postEvent(k_teleportOut); }
void SummonerSound::playTeleportIn()  { postEventDelayed(k_teleportIn, k_teleportInDelay); }
void SummonerSound::playRecover()     { postEvent(k_recover); }

void SummonerSound::onMovementStarted()
{
    if (m_crawlLoopActive)
    {
        return;
    }
    postEvent(k_crawlStart);
    m_crawlLoopActive = true;
}

void SummonerSound::onMovementStopped()
{
    if (!m_crawlLoopActive)
    {
        return;
    }
    postEvent(k_crawlStop);
    m_crawlLoopActive = false;
}

void SummonerSound::stopAllLoops()
{
    EnemySound::stopAllLoops();

    if (m_crawlLoopActive)
    {
        postEvent(k_crawlStop);
        m_crawlLoopActive = false;
    }
}

void SummonerSound::getCandidateBanks(const char* const*& outBanks, int& outCount) const
{
    outBanks = k_banks;
    outCount = static_cast<int>(sizeof(k_banks) / sizeof(k_banks[0]));
}

const char* SummonerSound::evBasicTelegraph() const { return k_charge; }
const char* SummonerSound::evBasicImpact()    const { return nullptr; }
const char* SummonerSound::evHurt()           const { return k_hurt; }
const char* SummonerSound::evStun()           const { return k_stun; }
const char* SummonerSound::evDeath()          const { return k_death; }
const char* SummonerSound::evFootstep()       const { return nullptr; }

IMPLEMENT_SCRIPT(SummonerSound)

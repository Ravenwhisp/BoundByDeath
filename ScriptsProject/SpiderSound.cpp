#include "pch.h"
#include "SpiderSound.h"

#include "SpiderEnemyController.h"

#include <cstdlib>

namespace
{
    constexpr const char* k_bite      = "Play_Spider_Bite";
    constexpr const char* k_hurt      = "Play_Spider_Hurt";
    constexpr const char* k_stun      = "Play_Spider_Stun";
    constexpr const char* k_death     = "Play_Spider_Death";
    constexpr const char* k_footstep  = "Play_Spider_Footsteps";
    constexpr const char* k_chitter   = "Play_Spider_Chitter";

    // Level2 is where the spider events are packed, but Level1 reuses the controller and
    // the final boss summons spiders of its own.
    const char* const k_banks[] = { "Level2.bnk", "Level1.bnk", "LevelCommon.bnk", "BossLevel.bnk" };

    // Only the nearest spider chitters inside this window.
    constexpr const char* k_chitterGroup = "SpiderChitter";
    constexpr uint32_t    k_chitterCooldownMs = 1500;
}

IMPLEMENT_SCRIPT_FIELDS(SpiderSound,
    SERIALIZED_FLOAT(m_chitterMinInterval, "Chitter Min Interval", 0.5f, 30.0f, 0.1f),
    SERIALIZED_FLOAT(m_chitterMaxInterval, "Chitter Max Interval", 0.5f, 30.0f, 0.1f)
)

SpiderSound::SpiderSound(GameObject* owner)
    : EnemySound(owner)
{
}

void SpiderSound::Start()
{
    EnemySound::Start();

    m_controller = GameObjectAPI::findScript<SpiderEnemyController>(getOwner());
    scheduleNextChitter();
}

void SpiderSound::Update()
{
    EnemySound::Update();

    if (m_controller == nullptr || !m_controller->hasValidTarget())
    {
        return;
    }

    m_chitterTimer -= Time::getDeltaTime();
    if (m_chitterTimer <= 0.0f)
    {
        postEventGrouped(k_chitter, k_chitterGroup, k_chitterCooldownMs);
        scheduleNextChitter();
    }
}

void SpiderSound::scheduleNextChitter()
{
    const float low  = m_chitterMinInterval > 0.0f ? m_chitterMinInterval : 4.0f;
    const float high = m_chitterMaxInterval > low ? m_chitterMaxInterval : low + 1.0f;
    const float t = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);

    m_chitterTimer = low + t * (high - low);
}

void SpiderSound::getCandidateBanks(const char* const*& outBanks, int& outCount) const
{
    outBanks = k_banks;
    outCount = static_cast<int>(sizeof(k_banks) / sizeof(k_banks[0]));
}

const char* SpiderSound::evBasicTelegraph() const { return k_bite; }
const char* SpiderSound::evBasicImpact()    const { return nullptr; }
const char* SpiderSound::evHurt()           const { return k_hurt; }
const char* SpiderSound::evStun()           const { return k_stun; }
const char* SpiderSound::evDeath()          const { return k_death; }
const char* SpiderSound::evFootstep()       const { return k_footstep; }

IMPLEMENT_SCRIPT(SpiderSound)

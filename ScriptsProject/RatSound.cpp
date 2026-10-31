#include "pch.h"
#include "RatSound.h"

namespace
{
    constexpr const char* k_bank = "BoundByDeath.bnk";

    constexpr const char* k_event_squeak = "Play_Rat_Squeak";
    constexpr const char* k_event_scurry = "Play_Rat_Scurry";
}

IMPLEMENT_SCRIPT(RatSound)

IMPLEMENT_SCRIPT_FIELDS(RatSound,
    SERIALIZED_FLOAT(m_squeakCooldown, "Squeak Cooldown", 0.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_scurryCooldown, "Scurry Cooldown", 0.0f, 5.0f, 0.05f)
)

RatSound::RatSound(GameObject* owner)
    : Script(owner)
{
}

void RatSound::Start()
{
    m_source = AudioAPI::getSoundSourceComponent(m_owner);

    if (m_source == nullptr)
    {
        Debug::error("[RatSound] No SOUND_SOURCE component on '%s'.",
            GameObjectAPI::getName(m_owner));
    }
}

void RatSound::Update()
{
    const float dt = Time::getDeltaTime();

    if (m_squeakTimer > 0.0f)
    {
        m_squeakTimer -= dt;
    }

    if (m_scurryTimer > 0.0f)
    {
        m_scurryTimer -= dt;
    }
}

void RatSound::playSqueak()
{
    if (m_source == nullptr || m_squeakTimer > 0.0f)
    {
        return;
    }

    AudioAPI::postEvent(m_source, k_bank, k_event_squeak);
    m_squeakTimer = m_squeakCooldown;
}

void RatSound::playScurry()
{
    if (m_source == nullptr || m_scurryTimer > 0.0f)
    {
        return;
    }

    m_scurryID = AudioAPI::postEvent(m_source, k_bank, k_event_scurry);
    m_scurryTimer = m_scurryCooldown;
}

void RatSound::stop()
{
    if (m_source == nullptr || m_scurryID == 0)
    {
        return;
    }

    AudioAPI::stopEvent(m_source, m_scurryID);
    m_scurryID = 0;
}

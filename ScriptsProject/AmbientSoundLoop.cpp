#include "pch.h"
#include "AmbientSoundLoop.h"

namespace
{
    constexpr const char* k_bank = "LevelCommon.bnk";
}

IMPLEMENT_SCRIPT(AmbientSoundLoop)

IMPLEMENT_SCRIPT_FIELDS(AmbientSoundLoop,
    SERIALIZED_STRING(m_playEvent, "Play Event"),
    SERIALIZED_STRING(m_stopEvent, "Stop Event"),
    SERIALIZED_BOOL(m_playOnStart, "Play On Start")
)

AmbientSoundLoop::AmbientSoundLoop(GameObject* owner)
    : Script(owner)
{
}

void AmbientSoundLoop::Start()
{
    m_source = AudioAPI::getSoundSourceComponent(getOwner());

    if (m_playOnStart && !m_playEvent.empty())
    {
        m_wantsPlay = true;
        tryPlay();
    }
}

void AmbientSoundLoop::Update()
{
    // The bank may still be loading when Start runs, in which case Wwise silently drops
    // the event. Keep retrying for a few seconds instead of staying quiet forever.
    if (m_playingID != 0 || m_retriesLeft <= 0) return;
    if (!m_wantsPlay || m_playEvent.empty()) return;

    m_retryTimer -= Time::getDeltaTime();
    if (m_retryTimer > 0.0f) return;

    tryPlay();
}

void AmbientSoundLoop::tryPlay()
{
    m_retryTimer = 0.5f;
    --m_retriesLeft;

    // The emitter is resolved here and not only in Start, because a torch that is lit
    // during play starts the scene disabled, so its Start never runs.
    if (m_source == nullptr)
    {
        m_source = AudioAPI::getSoundSourceComponent(getOwner());
    }

    if (m_source == nullptr)
    {
        if (m_retriesLeft <= 0)
        {
            Debug::warn("[AmbientSoundLoop] No SOUND_SOURCE component on '%s'.",
                        GameObjectAPI::getName(getOwner()));
        }
        return;
    }

    m_playingID = AudioAPI::postEvent(m_source, k_bank, m_playEvent.c_str());

    if (m_playingID != 0)
    {
        Debug::log("[AmbientSoundLoop] '%s' posted on '%s' (playingID=%u)",
                   m_playEvent.c_str(), GameObjectAPI::getName(getOwner()), m_playingID);
    }
    else if (m_retriesLeft <= 0)
    {
        Debug::warn("[AmbientSoundLoop] '%s' on '%s' never posted. Bank '%s' loaded?",
                    m_playEvent.c_str(), GameObjectAPI::getName(getOwner()), k_bank);
    }
}

void AmbientSoundLoop::play()
{
    if (m_playingID != 0 || m_playEvent.empty())
    {
        return;
    }

    m_wantsPlay = true;
    m_retriesLeft = 20;
    m_retryTimer = 0.0f;
    tryPlay();
}

void AmbientSoundLoop::stop()
{
    if (m_source == nullptr || m_stopEvent.empty())
    {
        return;
    }

    AudioAPI::postEvent(m_source, k_bank, m_stopEvent.c_str());
    m_playingID = 0;
    m_wantsPlay = false;
}

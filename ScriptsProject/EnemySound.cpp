#include "pch.h"
#include "EnemySound.h"

namespace
{
    // The engine resolves events against one named bank, so an enemy that shows up in
    // more than one level has to try each. Level1 first, which is where the enemies that
    // use the default list live.
    const char* const k_defaultBanks[] = { "Level1.bnk", "Level2.bnk", "BossLevel.bnk" };

    // Minimum gap between hurt one-shots so continuous/overlapping damage can't
    // machine-gun the grunt.
    constexpr float k_hurtRetriggerCooldown = 0.25f;

    // Footstep cadence and the watchdog window after the last step before they stop.
    constexpr float k_footstepInterval = 0.40f;
    constexpr float k_movingWatchdog   = 0.15f;
}

EnemySound::EnemySound(GameObject* owner)
    : Script(owner)
{
}

void EnemySound::Start()
{
    m_source = AudioAPI::getSoundSourceComponent(getOwner());
    if (m_source == nullptr)
    {
        Debug::error("[EnemySound] No SOUND_SOURCE component on '%s'.",
                     GameObjectAPI::getName(getOwner()));
    }
}

void EnemySound::Update()
{
    const float dt = Time::getDeltaTime();

    for (size_t i = 0; i < m_pendingEvents.size(); )
    {
        m_pendingEvents[i].timeRemaining -= dt;
        if (m_pendingEvents[i].timeRemaining <= 0.0f)
        {
            postEvent(m_pendingEvents[i].eventName);
            m_pendingEvents[i] = m_pendingEvents.back();
            m_pendingEvents.pop_back();
        }
        else
        {
            ++i;
        }
    }

    if (m_hurtCooldownTimer > 0.0f)
    {
        m_hurtCooldownTimer -= dt;
    }

    m_quietTimer += dt;

    if (m_movingTimer > 0.0f)
    {
        if (!m_wasMoving)
        {
            m_wasMoving = true;
            onMovementStarted();
        }

        m_movingTimer -= dt;

        m_footstepTimer -= dt;
        if (m_footstepTimer <= 0.0f)
        {
            postEvent(evFootstep());
            m_footstepTimer = k_footstepInterval;
        }
    }
    else
    {
        if (m_wasMoving)
        {
            m_wasMoving = false;
            onMovementStopped();
        }

        // Reset so the first step right after starting to move plays immediately.
        m_footstepTimer = 0.0f;
    }
}

void EnemySound::getCandidateBanks(const char* const*& outBanks, int& outCount) const
{
    outBanks = k_defaultBanks;
    outCount = static_cast<int>(sizeof(k_defaultBanks) / sizeof(k_defaultBanks[0]));
}

uint32_t EnemySound::postEvent(const char* eventName)
{
    if (m_source == nullptr || eventName == nullptr)
    {
        return 0;
    }

    if (m_resolvedBank != nullptr)
    {
        return AudioAPI::postEvent(m_source, m_resolvedBank, eventName);
    }

    const char* const* banks = nullptr;
    int bankCount = 0;
    getCandidateBanks(banks, bankCount);

    for (int i = 0; i < bankCount; ++i)
    {
        const uint32_t playingID = AudioAPI::postEvent(m_source, banks[i], eventName);
        if (playingID != 0)
        {
            m_resolvedBank = banks[i];
            return playingID;
        }
    }

    // Unresolved on purpose: the bank may still be loading, so the next post retries.
    Debug::warn("[EnemySound] '%s' not found in any candidate bank for '%s'.",
                eventName, GameObjectAPI::getName(getOwner()));
    return 0;
}

void EnemySound::postEventDelayed(const char* eventName, float delay)
{
    if (eventName == nullptr)
    {
        return;
    }
    if (delay <= 0.0f)
    {
        postEvent(eventName);
        return;
    }
    m_pendingEvents.push_back({ eventName, delay });
}

void EnemySound::playBasicTelegraph() { m_quietTimer = 0.0f; postEvent(evBasicTelegraph()); }
void EnemySound::playBasicImpact()    { m_quietTimer = 0.0f; postEvent(evBasicImpact()); }

void EnemySound::playHurt()
{
    if (m_hurtCooldownTimer > 0.0f)
    {
        return; // debounced: continuous/overlapping damage can't machine-gun the grunt
    }
    m_quietTimer = 0.0f;
    postEvent(evHurt());
    m_hurtCooldownTimer = k_hurtRetriggerCooldown;
}

void EnemySound::playStun()  {
    m_quietTimer = 0.0f; postEvent(evStun()); }
void EnemySound::playDeath() {
    m_quietTimer = 0.0f; postEvent(evDeath()); }

void EnemySound::notifyMoving()
{
    m_movingTimer = k_movingWatchdog;
}

void EnemySound::stopAllLoops()
{
    m_pendingEvents.clear();

    if (m_wasMoving)
    {
        m_wasMoving = false;
        onMovementStopped();
    }

    m_movingTimer   = 0.0f;
    m_footstepTimer = 0.0f;
}

void EnemySound::postEventGrouped(const char* eventName, const char* groupName, uint32_t cooldownMs)
{
    if (m_source == nullptr || eventName == nullptr || groupName == nullptr)
    {
        return;
    }

    if (m_resolvedBank == nullptr)
    {
        // Grouping needs a bank name, so let a plain post resolve it first.
        postEvent(eventName);
        return;
    }

    float priority = 0.0f;
    GameObject* camera = SceneAPI::getDefaultCameraGameObject();
    Transform* emitterTransform = GameObjectAPI::getTransform(getOwner());
    Transform* cameraTransform = camera != nullptr ? GameObjectAPI::getTransform(camera) : nullptr;
    if (emitterTransform != nullptr && cameraTransform != nullptr)
    {
        priority = Vector3::DistanceSquared(TransformAPI::getGlobalPosition(emitterTransform),
                                            TransformAPI::getGlobalPosition(cameraTransform));
    }

    AudioAPI::queueGroupedEvent(m_source, m_resolvedBank, eventName, groupName, priority, cooldownMs);
}

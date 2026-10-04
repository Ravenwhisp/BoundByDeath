#include "pch.h"
#include "PaladinSound.h"

#include "MeleeEnemyController.h"

#include <cstdlib>

namespace
{
    constexpr const char* k_scream = "Play_Paladin_Scream";

    // Only the nearest paladin shouts inside this window.
    constexpr const char* k_screamGroup = "PaladinScream";
    constexpr uint32_t    k_screamCooldownMs = 2500;

    // Never shout within this long of a swing, a grunt or a hit.
    constexpr float k_screamQuietWindow = 1.5f;
    constexpr const char* k_basicSwing    = "Play_Paladin_Basic_Swing";
    constexpr const char* k_basicImpact   = "Play_Paladin_Basic_Impact";
    constexpr const char* k_hurt          = "Play_Paladin_Hurt";
    constexpr const char* k_stun          = "Play_Paladin_Stun";
    constexpr const char* k_death         = "Play_Paladin_Death";
    constexpr const char* k_footstep      = "Play_PaladinFootsteps";

    constexpr const char* k_chargeStart   = "Play_Paladin_Charge_Start";
    constexpr const char* k_chargeLoop     = "Play_Paladin_Charge_Loop";
    constexpr const char* k_chargeLoopStop = "Stop_Paladin_Charge_Loop";
    constexpr const char* k_chargeImpact  = "Play_Paladin_Charge_Impact";
}

PaladinSound::PaladinSound(GameObject* owner)
    : EnemySound(owner)
{
}

const char* PaladinSound::evBasicTelegraph() const { return k_basicSwing; }
const char* PaladinSound::evBasicImpact()    const { return k_basicImpact; }
const char* PaladinSound::evHurt()           const { return k_hurt; }
const char* PaladinSound::evStun()           const { return k_stun; }
const char* PaladinSound::evDeath()          const { return k_death; }
const char* PaladinSound::evFootstep()       const { return k_footstep; }

void PaladinSound::playChargeStart() { postEvent(k_chargeStart); }

void PaladinSound::startChargeLoop()
{
    if (m_chargeLoopActive)
    {
        return;
    }
    m_chargeLoopActive = true;
    m_chargeLoopID = postEvent(k_chargeLoop);
}

void PaladinSound::stopChargeLoop()
{
    if (!m_chargeLoopActive)
    {
        return;
    }
    m_chargeLoopActive = false;
    m_chargeLoopID = 0;
    postEvent(k_chargeLoopStop);
}

void PaladinSound::playChargeImpact() { postEvent(k_chargeImpact); }

void PaladinSound::stopAllLoops()
{
    EnemySound::stopAllLoops();
    stopChargeLoop();
}

void PaladinSound::playScream(float delay)
{
    postEventDelayed(k_scream, delay);
}

void PaladinSound::Start()
{
    EnemySound::Start();

    m_controller = GameObjectAPI::findScript<MeleeEnemyController>(getOwner());
    scheduleNextScream();
}

void PaladinSound::Update()
{
    EnemySound::Update();

    if (m_controller == nullptr || !m_controller->hasValidTarget() || m_controller->isStunned())
    {
        return;
    }

    m_screamTimer -= Time::getDeltaTime();
    if (m_screamTimer > 0.0f)
    {
        return;
    }

    if (secondsSinceLastAction() < k_screamQuietWindow)
    {
        return;   // mid-swing, try again next frame
    }

    postEventGrouped(k_scream, k_screamGroup, k_screamCooldownMs);
    scheduleNextScream();
}

void PaladinSound::scheduleNextScream()
{
    const float low  = m_screamMinInterval > 0.0f ? m_screamMinInterval : 6.0f;
    const float high = m_screamMaxInterval > low ? m_screamMaxInterval : low + 1.0f;
    const float t = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);

    m_screamTimer = low + t * (high - low);
}

IMPLEMENT_SCRIPT_FIELDS(PaladinSound,
    SERIALIZED_FLOAT(m_screamMinInterval, "Scream Min Interval", 0.5f, 60.0f, 0.1f),
    SERIALIZED_FLOAT(m_screamMaxInterval, "Scream Max Interval", 0.5f, 60.0f, 0.1f)
)

IMPLEMENT_SCRIPT(PaladinSound)

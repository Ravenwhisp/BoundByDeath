#include "pch.h"
#include "DialogueManager.h"

#include "Bound.h"
#include "Damageable.h"
#include "DeathCharacter.h"
#include "LyrielCharacter.h"
#include "PlayerState.h"

#include <cstdlib>

namespace
{
    constexpr const char* k_bank = "DialogueCommon.bnk";

    // The duration is the longest take of each container, so the queue knows how long to
    // wait before letting anyone else speak.
    struct Bark { const char* event; float duration; };

    constexpr Bark k_deathTakesDamage  { "Play_Death_Death_Takes_Damage", 1.96f };
    constexpr Bark k_deathLowHealth    { "Play_Death_Death_Low_Health",   2.51f };
    constexpr Bark k_deathDowned       { "Play_Death_Death_Knocked_Out",  2.51f };
    constexpr Bark k_deathRevived      { "Play_Death_Death_Reviving",     2.51f };
    constexpr Bark k_deathOnLyrielDown { "Play_Death_Lyriel_Knocked_Out", 2.19f };

    constexpr Bark k_lyrielTakesDamage { "Play_Lyriel_Lyriel_Takes_Damage", 1.65f };
    constexpr Bark k_lyrielLowHealth   { "Play_Lyriel_Lyriel_Low_Health",   2.04f };
    constexpr Bark k_lyrielDowned      { "Play_Lyriel_Lyriel_Knocked_Out",  2.43f };
    constexpr Bark k_lyrielRevived     { "Play_Lyriel_Lyriel_Reviving",     2.12f };
    constexpr Bark k_lyrielOnDeathDown { "Play_Lyriel_Death_Knocked_Out",   1.80f };

    // Two line exchanges. firstIsLyriel says who opens, so the answer comes from the other.
    struct Exchange
    {
        const char* firstEvent;
        float       firstDuration;
        bool        firstIsLyriel;
        const char* secondEvent;
        float       secondDuration;
    };

    constexpr Exchange k_firstSeparation {
        "Play_First_Bound_Separation_1_Lyriel", 3.08f, true,
        "Play_First_Bound_Separation_2_Death",  3.63f };

    constexpr Exchange k_repeatedSeparations[] = {
        { "Play_D1_Repeated_Bound_Separation_1_Lyriel", 2.19f, true,
          "Play_D1_Repeated_Bound_Separation_2_Death",  2.43f },
        { "Play_D2_Repeated_Bound_Separation_1_Death",  2.35f, false,
          "Play_D2_Repeated_Bound_Separation_2_Lyriel", 1.96f },
        { "Play_D3_Repeated_Bound_Separation_1_Lyriel", 1.57f, true,
          "Play_D3_Repeated_Bound_Separation_2_Death",  2.19f },
    };

    float randomUnit()
    {
        return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    }
}

IMPLEMENT_SCRIPT_FIELDS(DialogueManager,
    SERIALIZED_FLOAT(m_lowHealthPercent,    "Low Health Percent",    0.0f, 1.0f,   0.01f),
    SERIALIZED_FLOAT(m_takesDamageChance,   "Takes Damage Chance",   0.0f, 1.0f,   0.01f),
    SERIALIZED_FLOAT(m_takesDamageCooldown, "Takes Damage Cooldown", 0.0f, 120.0f, 0.5f),
    SERIALIZED_FLOAT(m_lowHealthCooldown,   "Low Health Cooldown",   0.0f, 120.0f, 0.5f),
    SERIALIZED_FLOAT(m_separationCooldown,  "Separation Cooldown",   0.0f, 180.0f, 0.5f),
    SERIALIZED_FLOAT(m_lineGap,             "Gap Between Lines",     0.0f, 3.0f,   0.05f)
)

DialogueManager::DialogueManager(GameObject* owner)
    : Script(owner)
{
}

void DialogueManager::resolveSpeaker(Speaker& speaker, GameObject* object)
{
    speaker.object = object;
    if (object == nullptr)
    {
        return;
    }

    speaker.source     = AudioAPI::getSoundSourceComponent(object);
    speaker.damageable = GameObjectAPI::findScript<Damageable>(object);
    speaker.state      = GameObjectAPI::findScript<PlayerState>(object);

    if (speaker.source == nullptr)
    {
        Debug::warn("[DialogueManager] '%s' has no SOUND_SOURCE, it cannot speak.",
                    GameObjectAPI::getName(object));
    }
}

void DialogueManager::Start()
{
    for (GameObject* object : SceneAPI::findAllGameObjectsWithScript<DeathCharacter>())
    {
        resolveSpeaker(m_death, object);
        break;
    }

    for (GameObject* object : SceneAPI::findAllGameObjectsWithScript<LyrielCharacter>())
    {
        resolveSpeaker(m_lyriel, object);
        break;
    }

    for (GameObject* object : SceneAPI::findAllGameObjectsWithScript<Bound>())
    {
        m_bound = GameObjectAPI::findScript<Bound>(object);
        if (m_bound != nullptr)
        {
            break;
        }
    }
}

bool DialogueManager::busy() const
{
    return m_lineTimer > 0.0f || !m_queue.empty();
}

void DialogueManager::say(const Speaker& speaker, const char* eventName, float duration)
{
    if (speaker.source == nullptr || eventName == nullptr)
    {
        return;
    }

    m_queue.push_back({ speaker.source, eventName, duration });
}

void DialogueManager::updateQueue(float dt)
{
    if (m_lineTimer > 0.0f)
    {
        m_lineTimer -= dt;
        return;
    }

    if (m_queue.empty())
    {
        return;
    }

    const Line line = m_queue.front();
    m_queue.erase(m_queue.begin());

    AudioAPI::postEvent(line.source, k_bank, line.event);
    m_lineTimer = line.duration + m_lineGap;
}

void DialogueManager::updateSpeaker(Speaker& self, Speaker& partner, bool isDeath, float dt)
{
    if (self.damageable == nullptr || self.state == nullptr)
    {
        return;
    }

    const float hp = self.damageable->getCurrentHp();
    const float percent = self.damageable->getHpPercent();
    const bool downed = self.state->isDowned();

    const bool tookDamage = self.lastHp >= 0.0f && hp < self.lastHp - 0.01f;
    self.lastHp = hp;

    // Going down and getting back up are rare, so they always speak, and the partner
    // answers once the first line is out of the way.
    if (downed && !self.wasDowned)
    {
        self.wasDowned = true;
        if (!busy())
        {
            say(self, isDeath ? k_deathDowned.event : k_lyrielDowned.event,
                      isDeath ? k_deathDowned.duration : k_lyrielDowned.duration);
            say(partner, isDeath ? k_lyrielOnDeathDown.event : k_deathOnLyrielDown.event,
                         isDeath ? k_lyrielOnDeathDown.duration : k_deathOnLyrielDown.duration);
        }
        return;
    }

    if (!downed && self.wasDowned)
    {
        self.wasDowned = false;
        if (!busy())
        {
            say(self, isDeath ? k_deathRevived.event : k_lyrielRevived.event,
                      isDeath ? k_deathRevived.duration : k_lyrielRevived.duration);
        }
        return;
    }

    if (downed)
    {
        return;
    }

    // Crossing the threshold, not sitting under it, so it does not nag while low.
    const bool lowNow = percent <= m_lowHealthPercent;
    if (lowNow && !self.wasLowHp && self.lowHealthCooldown <= 0.0f && !busy())
    {
        self.lowHealthCooldown = m_lowHealthCooldown;
        say(self, isDeath ? k_deathLowHealth.event : k_lyrielLowHealth.event,
                  isDeath ? k_deathLowHealth.duration : k_lyrielLowHealth.duration);
    }
    self.wasLowHp = lowNow;

    // Taking a hit happens constantly, so it only speaks once in a while.
    if (tookDamage && self.damageCooldown <= 0.0f && !busy()
        && randomUnit() <= m_takesDamageChance)
    {
        self.damageCooldown = m_takesDamageCooldown;
        say(self, isDeath ? k_deathTakesDamage.event : k_lyrielTakesDamage.event,
                  isDeath ? k_deathTakesDamage.duration : k_lyrielTakesDamage.duration);
    }
}

void DialogueManager::updateSeparation(float dt)
{
    if (m_bound == nullptr)
    {
        return;
    }

    const bool separated = m_bound->m_currentRadius >= m_bound->m_showBoundDistance;

    if (separated && !m_wasSeparated && m_separationCooldownTimer <= 0.0f && !busy())
    {
        m_separationCooldownTimer = m_separationCooldown;

        const Exchange* exchange = &k_firstSeparation;
        if (m_firstSeparationDone)
        {
            const int count = static_cast<int>(sizeof(k_repeatedSeparations) / sizeof(k_repeatedSeparations[0]));
            exchange = &k_repeatedSeparations[std::rand() % count];
        }
        m_firstSeparationDone = true;

        const Speaker& opener   = exchange->firstIsLyriel ? m_lyriel : m_death;
        const Speaker& answerer = exchange->firstIsLyriel ? m_death  : m_lyriel;

        say(opener,   exchange->firstEvent,  exchange->firstDuration);
        say(answerer, exchange->secondEvent, exchange->secondDuration);
    }

    m_wasSeparated = separated;
}

void DialogueManager::Update()
{
    const float dt = Time::getDeltaTime();

    if (m_death.damageCooldown > 0.0f)     m_death.damageCooldown -= dt;
    if (m_death.lowHealthCooldown > 0.0f)  m_death.lowHealthCooldown -= dt;
    if (m_lyriel.damageCooldown > 0.0f)    m_lyriel.damageCooldown -= dt;
    if (m_lyriel.lowHealthCooldown > 0.0f) m_lyriel.lowHealthCooldown -= dt;
    if (m_separationCooldownTimer > 0.0f)  m_separationCooldownTimer -= dt;

    // Order is drawn each frame. Evaluating one of them first every time meant that when
    // both were hit at once, which the bound damage and any area attack do, the same one
    // always claimed the line and the other was never heard.
    if (randomUnit() < 0.5f)
    {
        updateSpeaker(m_death,  m_lyriel, true,  dt);
        updateSpeaker(m_lyriel, m_death,  false, dt);
    }
    else
    {
        updateSpeaker(m_lyriel, m_death,  false, dt);
        updateSpeaker(m_death,  m_lyriel, true,  dt);
    }
    updateSeparation(dt);
    updateQueue(dt);
}

IMPLEMENT_SCRIPT(DialogueManager)

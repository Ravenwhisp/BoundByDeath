#include "pch.h"
#include "LinkedDeathHandler.h"
#include "Damageable.h"
#include "EnemyDeathHandler.h"

IMPLEMENT_SCRIPT_FIELDS(LinkedDeathHandler,
    SERIALIZED_COMPONENT_REF(m_linkedPartner, "Linked Partner", ComponentType::SCRIPT),
    SERIALIZED_FLOAT(m_reviveDelay, "Revive Delay", 0.0f, 30.0f, 0.1f),
    SERIALIZED_FLOAT(m_reviveHpPercent, "Revive HP %", 0.0f, 1.0f, 0.05f),
)

LinkedDeathHandler::LinkedDeathHandler(GameObject* owner)
    : Script(owner)
{
}

void LinkedDeathHandler::Start()
{
    m_damageable = GameObjectAPI::findScript<Damageable>(m_owner);
    if (m_damageable == nullptr)
    {
        Debug::warn("LinkedDeathHandler on '%s' could not find Damageable.", GameObjectAPI::getName(m_owner));
    }

    m_deathHandler = GameObjectAPI::findScript<EnemyDeathHandler>(m_owner);
    if (m_deathHandler == nullptr)
    {
        Debug::warn("LinkedDeathHandler on '%s' could not find EnemyDeathHandler.", GameObjectAPI::getName(m_owner));
    }

    auto reference = m_linkedPartner.getReferencedComponent();
    m_partnerHandler = GameObjectAPI::findScript<LinkedDeathHandler>(reference->getOwner());
    if (m_partnerHandler == nullptr)
    {
        Debug::warn("LinkedDeathHandler on '%s' could not find linked partner.", GameObjectAPI::getName(m_owner));
    }
}

void LinkedDeathHandler::Update()
{
    if (m_damageable == nullptr || m_deathHandler == nullptr)
        return;

    if (m_isWaitingForPartner)
    {
        m_deathTimer -= Time::getDeltaTime();

        if (m_partnerHandler != nullptr && m_partnerHandler->m_damageable != nullptr && m_partnerHandler->m_damageable->isDead())
        {
            m_deathHandler->m_canProcessDeath = true;
            m_isWaitingForPartner = false;
            Debug::log("[LinkedDeath] %s: partner died in time! Allowing death.",
                GameObjectAPI::getName(m_owner));
        }
        else if (m_deathTimer <= 0.0f)
        {
            float reviveHp = m_damageable->getMaxHp() * m_reviveHpPercent;
            m_damageable->revive(reviveHp);
            m_deathHandler->m_canProcessDeath = true;
            m_isWaitingForPartner = false;
            m_hasProcessedDeath = false;
            Debug::log("[LinkedDeath] %s: revive timer expired. Revived with %.1f HP.",
                GameObjectAPI::getName(m_owner), reviveHp);
        }

        return;
    }

    if (m_hasProcessedDeath)
        return;

    if (m_damageable->isDead())
    {
        m_hasProcessedDeath = true;

        if (m_partnerHandler != nullptr && m_partnerHandler->m_damageable != nullptr && m_partnerHandler->m_damageable->isDead())
        {
            Debug::log("[LinkedDeath] %s: linked partner already dead. Allowing death.",
                GameObjectAPI::getName(m_owner));
            return;
        }

        m_deathHandler->m_canProcessDeath = false;
        m_isWaitingForPartner = true;
        m_deathTimer = m_reviveDelay;
        Debug::log("[LinkedDeath] %s: died before partner. Waiting %.1fs for linked death.",
            GameObjectAPI::getName(m_owner), m_reviveDelay);

        if (m_partnerHandler != nullptr)
        {
            m_partnerHandler->onLinkedPartnerDied();
        }
    }
}

void LinkedDeathHandler::onLinkedPartnerDied()
{
    if (!m_hasProcessedDeath && m_damageable != nullptr && !m_damageable->isDead())
    {
        Debug::log("[LinkedDeath] %s: partner died but I'm still alive. No action needed.",
            GameObjectAPI::getName(m_owner));
    }
}

IMPLEMENT_SCRIPT(LinkedDeathHandler)

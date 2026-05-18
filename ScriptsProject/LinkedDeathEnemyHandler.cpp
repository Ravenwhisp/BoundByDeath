#include "pch.h"
#include "LinkedDeathEnemyHandler.h"

#include "Damageable.h"

IMPLEMENT_SCRIPT_FIELDS_INHERITED(LinkedDeathEnemyHandler, EnemyDeathHandler,
    SERIALIZED_COMPONENT_REF(m_linkedPartner, "Linked Partner", ComponentType::TRANSFORM),
    SERIALIZED_FLOAT(m_graceWindow, "Grace Window", 0.0f, 30.0f, 0.1f),
    SERIALIZED_FLOAT(m_reviveHpPercent, "Revive HP %", 0.0f, 1.0f, 0.05f)
)

LinkedDeathEnemyHandler::LinkedDeathEnemyHandler(GameObject* owner)
    : EnemyDeathHandler(owner)
{
}

void LinkedDeathEnemyHandler::Start()
{
    EnemyDeathHandler::Start();

    Transform* partnerTransform = m_linkedPartner.getReferencedComponent();
    if (partnerTransform == nullptr)
    {
        Debug::warn("LinkedDeathEnemyHandler on '%s' is missing Linked Partner.", GameObjectAPI::getName(m_owner));
        return;
    }

    GameObject* partnerObject = ComponentAPI::getOwner(partnerTransform);
    if (partnerObject == nullptr)
    {
        Debug::warn("LinkedDeathEnemyHandler on '%s' has an invalid Linked Partner owner.", GameObjectAPI::getName(m_owner));
        return;
    }

    m_partnerHandler = GameObjectAPI::findScript<LinkedDeathEnemyHandler>(partnerObject);
    if (m_partnerHandler == nullptr)
    {
        Debug::warn("LinkedDeathEnemyHandler on '%s' could not find LinkedDeathEnemyHandler on partner '%s'.",
            GameObjectAPI::getName(m_owner), GameObjectAPI::getName(partnerObject));
    }
}

bool LinkedDeathEnemyHandler::canStartDeathProcessing() const
{
    if (!EnemyDeathHandler::canStartDeathProcessing())
    {
        return false;
    }

    if (m_partnerHandler == nullptr)
    {
        return true;
    }

    return isPartnerDead();
}

void LinkedDeathEnemyHandler::onDeathProcessingBlocked(float dt)
{
    if (m_partnerHandler == nullptr || m_damageable == nullptr)
    {
        return;
    }

    if (!m_isWaitingForPartner)
    {
        m_isWaitingForPartner = true;
        m_pendingDeathTimer = m_graceWindow;
        Debug::log("[LinkedDeath] %s died first. Waiting %.1fs for partner.",
            GameObjectAPI::getName(m_owner), m_graceWindow);
    }

    m_pendingDeathTimer -= dt;
    if (m_pendingDeathTimer > 0.0f)
    {
        return;
    }

    const float reviveHp = m_damageable->getMaxHp() * m_reviveHpPercent;
    clearPendingState();
    m_damageable->revive(reviveHp);

    Debug::log("[LinkedDeath] %s revived after grace window expired. HP: %.1f",
        GameObjectAPI::getName(m_owner), reviveHp);
}

void LinkedDeathEnemyHandler::onDeathProcessed()
{
    clearPendingState();
}

void LinkedDeathEnemyHandler::onDeathStateReset()
{
    clearPendingState();
}

bool LinkedDeathEnemyHandler::isPartnerDead() const
{
    return m_partnerHandler != nullptr && m_partnerHandler->m_damageable != nullptr && m_partnerHandler->m_damageable->isDead();
}

void LinkedDeathEnemyHandler::clearPendingState()
{
    m_isWaitingForPartner = false;
    m_pendingDeathTimer = 0.0f;
}

IMPLEMENT_SCRIPT(LinkedDeathEnemyHandler)

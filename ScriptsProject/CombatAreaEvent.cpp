#include "pch.h"
#include "CombatAreaEvent.h"

#include "GameplayEventTrigger.h"
#include "Damageable.h"

IMPLEMENT_SCRIPT_FIELDS(CombatAreaEvent,
    SERIALIZED_COMPONENT_REF_VECTOR(m_enemies, "Enemies", ComponentType::TRANSFORM),
    SERIALIZED_COMPONENT_REF(m_entranceBlocker, "Entrance Blocker", ComponentType::TRANSFORM),
    SERIALIZED_COMPONENT_REF(m_exitBlocker, "Exit Blocker", ComponentType::TRANSFORM)
)

CombatAreaEvent::CombatAreaEvent(GameObject* owner)
    : GameplayEventAction(owner)
{
}

void CombatAreaEvent::Update()
{
    if (!m_isActive || m_hasCompleted)
    {
        return;
    }

    if (!areAllEnemiesDead())
    {
        return;
    }

    openArea();

    m_isActive = false;
    m_hasCompleted = true;
}

void CombatAreaEvent::executeEvent(GameplayEventTrigger* trigger)
{
    if (m_hasCompleted)
    {
        return;
    }

    closeArea();

    m_isActive = true;
}

void CombatAreaEvent::closeArea()
{
    setBlockerState(m_entranceBlocker, true);
    setBlockerState(m_exitBlocker, true);
}

void CombatAreaEvent::openArea()
{
    setBlockerState(m_entranceBlocker, false);
    setBlockerState(m_exitBlocker, false);
}

void CombatAreaEvent::setBlockerState(const ScriptComponentRef<Transform>& blockerTransformRef, bool blocked)
{
    Transform* blockerTransform = blockerTransformRef.getReferencedComponent();

    if (blockerTransform == nullptr)
    {
        Debug::warn("CombatAreaEvent on '%s' has a missing blocker reference.", GameObjectAPI::getName(getOwner()));
        return;
    }

    GameObject* blockerObject = ComponentAPI::getOwner(blockerTransform);

    if (blockerObject == nullptr)
    {
        return;
    }

    auto* runtimeBlocker = NavigationAPI::getRuntimeBlockerComponent(blockerObject);

    if (runtimeBlocker == nullptr)
    {
        Debug::warn("CombatAreaEvent on '%s' has blocker object '%s' without NavRuntimeBlocker.", GameObjectAPI::getName(getOwner()), GameObjectAPI::getName(blockerObject));
        return;
    }

    NavigationAPI::setBlocked(runtimeBlocker, blocked);
}

bool CombatAreaEvent::areAllEnemiesDead() const
{
    for (const ScriptComponentRef<Transform>& enemyTransformRef : m_enemies)
    {
        if (!isEnemyDead(enemyTransformRef))
        {
            return false;
        }
    }

    return true;
}

bool CombatAreaEvent::isEnemyDead(const ScriptComponentRef<Transform>& enemyTransformRef) const
{
    Transform* enemyTransform = enemyTransformRef.getReferencedComponent();

    if (enemyTransform == nullptr)
    {
        return true;
    }

    GameObject* enemyObject = ComponentAPI::getOwner(enemyTransform);

    if (enemyObject == nullptr)
    {
        return true;
    }

    Damageable* damageable = GameObjectAPI::findScript<Damageable>(enemyObject);

    if (damageable == nullptr)
    {
        Debug::warn("CombatAreaEvent on '%s' has enemy '%s' without Damageable.", GameObjectAPI::getName(getOwner()), GameObjectAPI::getName(enemyObject));

        return true;
    }

    return damageable->isDead();
}

IMPLEMENT_SCRIPT(CombatAreaEvent)
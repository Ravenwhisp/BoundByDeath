#include "pch.h"
#include "BreakableObject.h"

IMPLEMENT_SCRIPT_FIELDS(BreakableObject,
    SERIALIZED_COMPONENT_REF(m_normalObjectTransformComponent, "Normal Object", ComponentType::TRANSFORM),
    SERIALIZED_COMPONENT_REF(m_brokenObjectTransformComponent, "Broken Object", ComponentType::TRANSFORM)
)

BreakableObject::BreakableObject(GameObject* owner)
    : Script(owner)
{
}

void BreakableObject::Start()
{
    m_isBroken = false;

    m_normalObjectTransform = m_normalObjectTransformComponent.getReferencedComponent();
    m_brokenObjectTransform = m_brokenObjectTransformComponent.getReferencedComponent();

    if (m_normalObjectTransform != nullptr)
    {
        GameObject* normalObject = ComponentAPI::getOwner(m_normalObjectTransform);
        GameObjectAPI::setActive(normalObject, true);
    }

    if (m_brokenObjectTransform != nullptr)
    {
        GameObject* brokenObject = ComponentAPI::getOwner(m_brokenObjectTransform);
        GameObjectAPI::setActive(brokenObject, false);
    }
}

void BreakableObject::breakObject()
{
    if (m_isBroken)
    {
        return;
    }

    m_isBroken = true;

    //Transform* normalTransform = m_normalObjectTransformComponent.getReferencedComponent();
    //Transform* brokenTransform = m_brokenObjectTransformComponent.getReferencedComponent();

    if (m_normalObjectTransform != nullptr)
    {
        GameObject* normalObject = ComponentAPI::getOwner(m_normalObjectTransform);
        GameObjectAPI::setActive(normalObject, false);
    }

    if (m_brokenObjectTransform != nullptr)
    {
        GameObject* brokenObject = ComponentAPI::getOwner(m_brokenObjectTransform);
        GameObjectAPI::setActive(brokenObject, true);
    }

    Debug::log("[BreakableObject] '%s' broke.", GameObjectAPI::getName(getOwner()));
}

IMPLEMENT_SCRIPT(BreakableObject)
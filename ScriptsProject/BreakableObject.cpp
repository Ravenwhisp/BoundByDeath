#include "pch.h"
#include "BreakableObject.h"

IMPLEMENT_SCRIPT_FIELDS(BreakableObject,
    SERIALIZED_COMPONENT_REF(m_normalObjectTransform, "Normal Object", ComponentType::TRANSFORM),
    SERIALIZED_COMPONENT_REF(m_brokenObjectTransform, "Broken Object", ComponentType::TRANSFORM)
)

BreakableObject::BreakableObject(GameObject* owner)
    : Script(owner)
{
}

void BreakableObject::Start()
{
    m_isBroken = false;

    Transform* normalTransform = m_normalObjectTransform.getReferencedComponent();
    Transform* brokenTransform = m_brokenObjectTransform.getReferencedComponent();

    if (normalTransform != nullptr)
    {
        GameObject* normalObject = ComponentAPI::getOwner(normalTransform);
        GameObjectAPI::setActive(normalObject, true);
    }

    if (brokenTransform != nullptr)
    {
        GameObject* brokenObject = ComponentAPI::getOwner(brokenTransform);
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

    Transform* normalTransform = m_normalObjectTransform.getReferencedComponent();
    Transform* brokenTransform = m_brokenObjectTransform.getReferencedComponent();

    if (normalTransform != nullptr)
    {
        GameObject* normalObject = ComponentAPI::getOwner(normalTransform);
        GameObjectAPI::setActive(normalObject, false);
    }

    if (brokenTransform != nullptr)
    {
        GameObject* brokenObject = ComponentAPI::getOwner(brokenTransform);
        GameObjectAPI::setActive(brokenObject, true);
    }

    Debug::log("[BreakableObject] '%s' broke.", GameObjectAPI::getName(getOwner()));
}

IMPLEMENT_SCRIPT(BreakableObject)
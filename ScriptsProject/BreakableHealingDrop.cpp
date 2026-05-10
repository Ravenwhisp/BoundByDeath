#include "pch.h"
#include "BreakableHealingDrop.h"

#include "HealthPickup.h"

IMPLEMENT_SCRIPT_FIELDS(BreakableHealingDrop,
    SERIALIZED_STRING(m_healthPickupPrefabPath, "Health Pickup Prefab Path")
)

BreakableHealingDrop::BreakableHealingDrop(GameObject* owner)
    : BreakableObject(owner)
{
}

void BreakableHealingDrop::Start()
{
    BreakableObject::Start(); //si no hay nada aqui, quitamos el start
}

void BreakableHealingDrop::Update()
{
}

void BreakableHealingDrop::onBreak()
{
    if (m_healthPickupPrefabPath.empty())
    {
		Debug::warn("[BreakableHealingDrop] '%s' has no health pickup prefab path set.", GameObjectAPI::getName(getOwner()));
        return;
    }

    GameObjectAPI::instantiatePrefab(m_healthPickupPrefabPath.c_str(), 
        TransformAPI::getPosition(GameObjectAPI::getTransform(getOwner())), 
        Vector3::Zero);

    BreakableObject::breakObject();
}

IMPLEMENT_SCRIPT(BreakableHealingDrop)
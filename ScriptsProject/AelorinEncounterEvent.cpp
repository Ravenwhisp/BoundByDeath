#include "pch.h"
#include "AelorinEncounterEvent.h"

#include "GameplayEventTrigger.h"
#include "AelorinCinematics.h"

IMPLEMENT_SCRIPT_FIELDS(AelorinEncounterEvent,
    SERIALIZED_COMPONENT_REF(m_bossTransform, "Boss Transform", ComponentType::TRANSFORM)
)

AelorinEncounterEvent::AelorinEncounterEvent(GameObject* owner)
    : GameplayEventAction(owner)
{
}

void AelorinEncounterEvent::executeEvent(GameplayEventTrigger* trigger)
{
    Transform* bossTransform = m_bossTransform.getReferencedComponent();
    if (!bossTransform)
    {
        Debug::warn("AelorinEncounterEvent on '%s' has no Boss Transform assigned.", GameObjectAPI::getName(getOwner()));
        return;
    }

    GameObject* bossObject = ComponentAPI::getOwner(bossTransform);
    if (!bossObject)
    {
        return;
    }

    AelorinCinematics* cinematics = GameObjectAPI::findScript<AelorinCinematics>(bossObject);
    if (!cinematics)
    {
        Debug::warn("AelorinEncounterEvent on '%s' could not find AelorinCinematics on the referenced boss.", GameObjectAPI::getName(getOwner()));
        return;
    }

    cinematics->requestEncounterStart();
}

IMPLEMENT_SCRIPT(AelorinEncounterEvent)

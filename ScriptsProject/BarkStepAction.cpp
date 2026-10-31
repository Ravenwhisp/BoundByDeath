#include "pch.h"
#include "BarkStepAction.h"

#include "BarkEvent.h"

IMPLEMENT_SCRIPT_FIELDS_INHERITED(BarkStepAction, CameraTransitionStepAction,
    SERIALIZED_COMPONENT_REF(m_barkObject, "Bark Object", ComponentType::TRANSFORM),
    SERIALIZED_BOOL(m_barkOnlyOnce, "Bark Only Once")
)

BarkStepAction::BarkStepAction(GameObject* owner)
    : CameraTransitionStepAction(owner)
{
}

void BarkStepAction::executeAction(CameraTransitionController* controller, CameraTransitionStep* step)
{
    if (m_barkOnlyOnce && m_hasPlayed)
    {
        return;
    }

    Transform* barkTransform = m_barkObject.getReferencedComponent();
    if (!barkTransform)
    {
        Debug::warn("BarkStepAction on '%s' has no Bark Object assigned.", GameObjectAPI::getName(getOwner()));
        return;
    }

    GameObject* barkObject = ComponentAPI::getOwner(barkTransform);
    if (!barkObject)
    {
        return;
    }

    BarkEvent* barkEvent = GameObjectAPI::findScript<BarkEvent>(barkObject);
    if (!barkEvent)
    {
        Debug::warn("BarkStepAction on '%s' could not find a BarkEvent on the referenced object.", GameObjectAPI::getName(getOwner()));
        return;
    }

    // Solo se consume el flanco si de verdad ha sonado: play() devuelve false cuando corre
    // su cooldown, falla el trigger chance o no hay BarkManager, y la linea se perderia.
    if (barkEvent->play())
    {
        m_hasPlayed = true;
    }
}

IMPLEMENT_SCRIPT(BarkStepAction)

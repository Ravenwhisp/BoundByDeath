#include "pch.h"
#include "PopUpEvent.h"

#include "GameplayEventTrigger.h"
#include "PopUpController.h"

IMPLEMENT_SCRIPT_FIELDS(PopUpEvent,
    SERIALIZED_COMPONENT_REF(m_popUpImage, "Pop Up Image", ComponentType::TRANSFORM2D),
    SERIALIZED_FLOAT(m_fadeInDuration, "Fade In Duration", 0.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_fadeOutDuration, "Fade Out Duration", 0.0f, 5.0f, 0.05f)
)

PopUpEvent::PopUpEvent(GameObject* owner)
    : GameplayEventAction(owner)
{
}

void PopUpEvent::executeEvent(GameplayEventTrigger* trigger)
{
    Transform2D* popUpImage = getPopUpImageTransform2D();

    if (popUpImage == nullptr)
    {
        Debug::warn("PopUpEvent on '%s' has no Pop Up Image assigned.", GameObjectAPI::getName(getOwner()));
        return;
    }

    PopUpController* popUpController = findPopUpController();

    if (popUpController == nullptr)
    {
        Debug::warn("PopUpEvent on '%s' could not find PopUpController in the scene.", GameObjectAPI::getName(getOwner()));
        return;
    }

    popUpController->startPopUp(this);
}

PopUpController* PopUpEvent::findPopUpController() const
{
    const std::vector<GameObject*> popUpControllerObjects = SceneAPI::findAllGameObjectsWithScript<PopUpController>();

    if (popUpControllerObjects.empty())
    {
        return nullptr;
    }

    return GameObjectAPI::findScript<PopUpController>(popUpControllerObjects[0]);
}

IMPLEMENT_SCRIPT(PopUpEvent)
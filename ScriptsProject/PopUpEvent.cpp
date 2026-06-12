#include "pch.h"
#include "PopUpEvent.h"

#include "GameplayEventTrigger.h"
#include "PopUpController.h"

PopUpEvent::PopUpEvent(GameObject* owner)
    : GameplayEventAction(owner)
{
}

void PopUpEvent::executeEvent(GameplayEventTrigger* trigger)
{
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
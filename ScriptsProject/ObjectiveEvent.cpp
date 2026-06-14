#include "pch.h"
#include "ObjectiveEvent.h"

#include "GameplayEventTrigger.h"
#include "PopUpController.h"

static const char* objectiveTypeNames[] =
{
    "None",
    "Movement",
    "Auto Attack",
    //"Charged Attack",
    //"Ability 1"
};

constexpr int objectiveTypeCount = 3;

IMPLEMENT_SCRIPT_FIELDS(ObjectiveEvent,
    SERIALIZED_ENUM_INT(m_objectiveType, "Objective Type", objectiveTypeNames, objectiveTypeCount),
    SERIALIZED_INT(m_targetPlayerIndex, "Target Player Index")
)

ObjectiveEvent::ObjectiveEvent(GameObject* owner)
    : GameplayEventAction(owner)
{
}

void ObjectiveEvent::executeEvent(GameplayEventTrigger* trigger)
{
    m_isActive = true;
    m_hasCompleted = false;
}

void ObjectiveEvent::Update()
{
    if (!m_isActive || m_hasCompleted)
    {
        return;
    }

    if (!isObjectiveCompleted())
    {
        return;
    }

    PopUpController* popUpController = findPopUpController();

    if (popUpController == nullptr)
    {
        Debug::warn("ObjectiveEvent on '%s' could not find PopUpController.", GameObjectAPI::getName(getOwner()));
        return;
    }

    popUpController->notifyObjectiveCompleted(getOwner());

    m_hasCompleted = true;
    m_isActive = false;
}

bool ObjectiveEvent::isObjectiveCompleted() const
{
    const ObjectiveType objectiveType = static_cast<ObjectiveType>(m_objectiveType);

    switch (objectiveType)
    {
    case ObjectiveType::Movement:
        return isMovementCompleted();

    case ObjectiveType::AutoAttack:
        return isAutoAttackCompleted();

    case ObjectiveType::None:
    default:
        return false;
    }
}

bool ObjectiveEvent::isMovementCompleted() const
{
    //Need to create this objective
    const Vector2 moveInput = Input::getMoveAxis(m_targetPlayerIndex);

    return moveInput.Length() > 0.1f;
}

bool ObjectiveEvent::isAutoAttackCompleted() const
{
    //Need to create this objective
    return false;
}

PopUpController* ObjectiveEvent::findPopUpController() const
{
    const std::vector<GameObject*> popUpControllerObjects = SceneAPI::findAllGameObjectsWithScript<PopUpController>();

    if (popUpControllerObjects.empty())
    {
        return nullptr;
    }

    return GameObjectAPI::findScript<PopUpController>(popUpControllerObjects[0]);
}

IMPLEMENT_SCRIPT(ObjectiveEvent)
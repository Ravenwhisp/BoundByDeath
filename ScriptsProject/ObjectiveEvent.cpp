#include "pch.h"
#include "ObjectiveEvent.h"

#include "GameplayEventTrigger.h"
#include "PopUpController.h"

#include "CharacterBase.h"
#include "PlayerMovement.h"
#include "AbilityBase.h"

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
    findTargetPlayer();

    m_initialBasicAttackUseCount = m_targetBasicAttack != nullptr ? m_targetBasicAttack->getSuccessfulUse() : 0;

    m_isActive = true;
    m_hasCompleted = false;
}

void ObjectiveEvent::Start()
{
    findTargetPlayer();
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
    if (m_targetPlayerMovement == nullptr)
    {
        return false;
    }

    return m_targetPlayerMovement->isMoving();
}

bool ObjectiveEvent::isAutoAttackCompleted() const
{
    if (m_targetBasicAttack == nullptr)
    {
        return false;
    }

    return m_targetBasicAttack->getSuccessfulUse() > m_initialBasicAttackUseCount;
}

void ObjectiveEvent::findTargetPlayer()
{
    m_targetCharacter = nullptr;
    m_targetPlayerMovement = nullptr;
    m_targetBasicAttack = nullptr;

    const std::vector<GameObject*> players = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER, true);

    for (GameObject* player : players)
    {
        if (player == nullptr)
        {
            continue;
        }

        CharacterBase* character = GameObjectAPI::findScript<CharacterBase>(player);

        if (character == nullptr)
        {
            continue;
        }

        if (character->getPlayerIndex() != m_targetPlayerIndex)
        {
            continue;
        }

        m_targetCharacter = character;
        m_targetPlayerMovement = GameObjectAPI::findScript<PlayerMovement>(player);
        m_targetBasicAttack = character->getBasicAttack();

        if (m_targetPlayerMovement == nullptr)
        {
            Debug::warn("ObjectiveEvent on '%s' could not find PlayerMovement on target player %d.", GameObjectAPI::getName(getOwner()), m_targetPlayerIndex);
        }

        if (m_targetBasicAttack == nullptr)
        {
            Debug::warn("ObjectiveEvent on '%s' could not find BasicAttack on target player %d.", GameObjectAPI::getName(getOwner()), m_targetPlayerIndex);
        }

        return;
    }

    Debug::warn("ObjectiveEvent on '%s' could not find player with index %d.", GameObjectAPI::getName(getOwner()), m_targetPlayerIndex);
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
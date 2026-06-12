#include "pch.h"
#include "TutorialUIEvent.h"

#include "GameplayEventTrigger.h"
#include "TutorialUIController.h"

static const char* tutorialUIEventTypeNames[] =
{
    "Side Info",
    "Collectible Popup"
};

constexpr int tutorialUIEventTypeCount = 2;

static const char* tutorialUITransitionTypeNames[] =
{
    "Fade",
    "Slide From Left",
    "Slide From Right"
};

constexpr int tutorialUITransitionTypeCount = 3;

static const char* tutorialUICloseModeNames[] =
{
    "Both Players Confirm",
    "Objective Completed"
};

constexpr int tutorialUICloseModeCount = 2;

IMPLEMENT_SCRIPT_FIELDS(TutorialUIEvent,
    SERIALIZED_COMPONENT_REF(m_tutorialUIImage, "Tutorial UI Image", ComponentType::TRANSFORM2D),
    SERIALIZED_ENUM_INT(m_eventType, "Event Type", tutorialUIEventTypeNames, tutorialUIEventTypeCount),
    SERIALIZED_ENUM_INT(m_transitionType, "Transition Type", tutorialUITransitionTypeNames, tutorialUITransitionTypeCount),
    SERIALIZED_ENUM_INT(m_closeMode, "Close Mode", tutorialUICloseModeNames, tutorialUICloseModeCount),
    SERIALIZED_FLOAT(m_showDuration, "Show Duration", 0.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_hideDuration, "Hide Duration", 0.0f, 5.0f, 0.05f)
)

TutorialUIEvent::TutorialUIEvent(GameObject* owner)
    : GameplayEventAction(owner)
{
}

void TutorialUIEvent::executeEvent(GameplayEventTrigger* trigger)
{
    Transform2D* tutorialUIImage = getTutorialUIImageTransform2D();

    if (tutorialUIImage == nullptr)
    {
        Debug::warn("TutorialUIEvent on '%s' has no Tutorial UI Image assigned.", GameObjectAPI::getName(getOwner()));
        return;
    }

    TutorialUIController* tutorialUIController = findTutorialUIController();

    if (tutorialUIController == nullptr)
    {
        Debug::warn("TutorialUIEvent on '%s' could not find TutorialUIController in the scene.", GameObjectAPI::getName(getOwner()));
        return;
    }

    tutorialUIController->startTutorialUI(this);
}

TutorialUIController* TutorialUIEvent::findTutorialUIController() const
{
    const std::vector<GameObject*> tutorialUIControllerObjects = SceneAPI::findAllGameObjectsWithScript<TutorialUIController>();

    if (tutorialUIControllerObjects.empty())
    {
        return nullptr;
    }

    return GameObjectAPI::findScript<TutorialUIController>(tutorialUIControllerObjects[0]);
}

IMPLEMENT_SCRIPT(TutorialUIEvent)
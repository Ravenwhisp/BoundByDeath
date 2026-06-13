#include "pch.h"
#include "TutorialUIEvent.h"

#include "GameplayEventTrigger.h"
#include "TutorialUIController.h"

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
    SERIALIZED_COMPONENT_REF_VECTOR(m_tutorialUIImages, "Tutorial UI Images", ComponentType::TRANSFORM2D),
    SERIALIZED_ENUM_INT(m_transitionType, "Transition Type", tutorialUITransitionTypeNames, tutorialUITransitionTypeCount),
    SERIALIZED_ENUM_INT(m_closeMode, "Close Mode", tutorialUICloseModeNames, tutorialUICloseModeCount),
    SERIALIZED_BOOL(m_lockGameplay, "Lock Gameplay"),
    SERIALIZED_FLOAT(m_showDuration, "Show Duration", 0.0f, 5.0f, 0.05f),
    SERIALIZED_FLOAT(m_hideDuration, "Hide Duration", 0.0f, 5.0f, 0.05f)
)

TutorialUIEvent::TutorialUIEvent(GameObject* owner)
    : GameplayEventAction(owner)
{
}

void TutorialUIEvent::executeEvent(GameplayEventTrigger* trigger)
{
    if (getTutorialUIImageCount() == 0)
    {
        Debug::warn("TutorialUIEvent on '%s' has no Tutorial UI Images assigned.", GameObjectAPI::getName(getOwner()));
        return;
    }

    if (getTutorialUIImageTransform2D(0) == nullptr)
    {
        Debug::warn("TutorialUIEvent on '%s' has an invalid first Tutorial UI Image.", GameObjectAPI::getName(getOwner()));
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

Transform2D* TutorialUIEvent::getTutorialUIImageTransform2D(int index) const
{
    if (index < 0 || index >= static_cast<int>(m_tutorialUIImages.size()))
    {
        return nullptr;
    }

    return m_tutorialUIImages[index].getReferencedComponent();
}

int TutorialUIEvent::getTutorialUIImageCount() const
{
    return static_cast<int>(m_tutorialUIImages.size());
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
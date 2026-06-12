#pragma once

#include "ScriptAPI.h"
#include "GameplayEventAction.h"

#include "Transform2D.h"

class GameplayEventTrigger;
class TutorialUIController;

enum class TutorialUIEventType
{
    SideInfo = 0,
    CollectiblePopup
};

enum class TutorialUITransitionType
{
    Fade = 0,
    SlideFromLeft,
    SlideFromRight
};

enum class TutorialUICloseMode
{
    BothPlayersConfirm = 0,
    ObjectiveCompleted
};

class TutorialUIEvent : public GameplayEventAction
{
    DECLARE_SCRIPT(TutorialUIEvent)

public:
    explicit TutorialUIEvent(GameObject* owner);

    void executeEvent(GameplayEventTrigger* trigger) override;

    ScriptFieldList getExposedFields() const override;

    Transform2D* getTutorialUIImageTransform2D() const { return m_tutorialUIImage.getReferencedComponent(); }

    TutorialUIEventType getEventType() const { return static_cast<TutorialUIEventType>(m_eventType); }
    TutorialUITransitionType getTransitionType() const { return static_cast<TutorialUITransitionType>(m_transitionType); }
    TutorialUICloseMode getCloseMode() const { return static_cast<TutorialUICloseMode>(m_closeMode); }

    float getShowDuration() const { return m_showDuration; }
    float getHideDuration() const { return m_hideDuration; }

    bool shouldLockGameplay() const { return m_lockGameplay; }

private:
    TutorialUIController* findTutorialUIController() const;

public:
    ScriptComponentRef<Transform2D> m_tutorialUIImage;

    int m_eventType = static_cast<int>(TutorialUIEventType::CollectiblePopup);
    int m_transitionType = static_cast<int>(TutorialUITransitionType::Fade);
    int m_closeMode = static_cast<int>(TutorialUICloseMode::BothPlayersConfirm);

    float m_showDuration = 0.25f;
    float m_hideDuration = 0.25f;

    bool m_lockGameplay = true;
};
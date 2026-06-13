#pragma once

#include "ScriptAPI.h"
#include "GameplayEventAction.h"

#include "Transform2D.h"

class GameplayEventTrigger;
class TutorialUIController;

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

    Transform2D* getTutorialUIImageTransform2D(int index) const;
    int getTutorialUIImageCount() const;

    TutorialUITransitionType getTransitionType() const { return static_cast<TutorialUITransitionType>(m_transitionType); }
    TutorialUICloseMode getCloseMode() const { return static_cast<TutorialUICloseMode>(m_closeMode); }

    float getShowDuration() const { return m_showDuration; }
    float getHideDuration() const { return m_hideDuration; }

    bool shouldLockGameplay() const { return m_lockGameplay; }

private:
    TutorialUIController* findTutorialUIController() const;

public:
    std::vector<ScriptComponentRef<Transform2D>> m_tutorialUIImages;

    int m_transitionType = static_cast<int>(TutorialUITransitionType::Fade);
    int m_closeMode = static_cast<int>(TutorialUICloseMode::BothPlayersConfirm);

    float m_showDuration = 0.25f;
    float m_hideDuration = 0.25f;

    bool m_lockGameplay = true;
};
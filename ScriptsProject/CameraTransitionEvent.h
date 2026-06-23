#pragma once

#include "ScriptAPI.h"
#include "GameplayEventAction.h"

class CameraTransitionController;
class GameplayEventTrigger;

enum class CameraTransitionMode
{
    TimedCinematic = 0,
    HoldWhileTriggered
};

class CameraTransitionEvent : public GameplayEventAction
{
    DECLARE_SCRIPT(CameraTransitionEvent)

public:
    explicit CameraTransitionEvent(GameObject* owner);

    void Start() override;

    void executeEvent(GameplayEventTrigger* trigger) override;
    void stopEvent(GameplayEventTrigger* trigger) override;

    ScriptFieldList getExposedFields() const override;

    const std::vector<Transform*>& getTargetPoints() const { return m_targetPoints; }
    Transform* getTargetPoint(int index) const;
    int getTargetPointCount() const { return static_cast<int>(m_targetPoints.size()); }

    float getPathDuration() const { return m_pathDuration; }
    float getHoldDuration() const { return m_holdDuration; }
    float getReturnDuration() const { return m_returnDuration; }

    bool usesFovTransition() const { return m_useFovTransition; }
    float getTargetFov() const { return m_targetFov; }

    CameraTransitionMode getTransitionMode() const { return static_cast<CameraTransitionMode>(m_transitionMode); }
    bool isTimedCinematicMode() const { return getTransitionMode() == CameraTransitionMode::TimedCinematic; }
    bool isHoldWhileTriggeredMode() const { return getTransitionMode() == CameraTransitionMode::HoldWhileTriggered; }

    bool shouldLockGameplayInput() const { return m_lockGameplayInput; }
    bool shouldMakePlayersInvulnerable() const { return m_makePlayersInvulnerable; }
    bool shouldFadeHud() const { return m_fadeHud; }

private:
    void findTargetPoints();
    Transform* findCameraPointsRoot() const;
    CameraTransitionController* findCameraTransitionController() const;

public:
    int m_transitionMode = static_cast<int>(CameraTransitionMode::TimedCinematic);

    float m_pathDuration = 1.5f;
    float m_holdDuration = 1.0f;
    float m_returnDuration = 1.5f;

    bool m_useFovTransition = false;
    float m_targetFov = 90.0f;

    bool m_lockGameplayInput = true;
    bool m_makePlayersInvulnerable = true;
    bool m_fadeHud = true;

private:
    std::vector<Transform*> m_targetPoints;
};
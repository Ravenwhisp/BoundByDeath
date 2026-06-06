#pragma once

#include "ScriptAPI.h"
#include "GameplayEventAction.h"

class CameraTransitionController;
class GameplayEventTrigger;

class CameraTransitionEvent : public GameplayEventAction
{
    DECLARE_SCRIPT(CameraTransitionEvent)

public:
    explicit CameraTransitionEvent(GameObject* owner);

    void Start() override;

    void executeEvent(GameplayEventTrigger* trigger) override;

    ScriptFieldList getExposedFields() const override;


    Transform* getTargetPoint() const { return m_targetPoint; }
    float getMoveToDuration() const { return m_moveToDuration; }
    float getHoldDuration() const { return m_holdDuration; }
    float getReturnDuration() const { return m_returnDuration; }

private:
    Transform* findTargetPoint() const;
    CameraTransitionController* findCameraTransitionController() const;

public:
    float m_moveToDuration = 1.5f;
    float m_holdDuration = 1.0f;
    float m_returnDuration = 1.5f;

private:
    Transform* m_targetPoint = nullptr;
};
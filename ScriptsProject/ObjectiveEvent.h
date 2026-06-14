#pragma once

#include "ScriptAPI.h"
#include "GameplayEventAction.h"

#include "ObjectiveTypes.h"

class GameplayEventTrigger;
class PopUpController;

class ObjectiveEvent : public GameplayEventAction
{
    DECLARE_SCRIPT(ObjectiveEvent)

public:
    explicit ObjectiveEvent(GameObject* owner);

    void Update() override;

    void executeEvent(GameplayEventTrigger* trigger) override;

    ScriptFieldList getExposedFields() const override;

private:
    bool isObjectiveCompleted() const;
    bool isMovementCompleted() const;
    bool isAutoAttackCompleted() const;

    PopUpController* findPopUpController() const;

private:
    bool m_isActive = false;
    bool m_hasCompleted = false;

public:
    int m_objectiveType = static_cast<int>(ObjectiveType::Movement);
    int m_targetPlayerIndex = 0;
};
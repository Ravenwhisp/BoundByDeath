#pragma once

#include "ScriptAPI.h"
#include "GameplayEventAction.h"

class GameplayEventTrigger;
class PopUpController;

class PopUpEvent : public GameplayEventAction
{
    DECLARE_SCRIPT(PopUpEvent)

public:
    explicit PopUpEvent(GameObject* owner);

    void executeEvent(GameplayEventTrigger* trigger) override;

private:
    PopUpController* findPopUpController() const;
};
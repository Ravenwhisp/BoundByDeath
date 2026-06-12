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

    ScriptFieldList getExposedFields() const override;

    Transform2D* getPopUpImageTransform2D() const { return m_popUpImage.getReferencedComponent(); }

    float getFadeInDuration() const { return m_fadeInDuration; }
    float getFadeOutDuration() const { return m_fadeOutDuration; }

private:
    PopUpController* findPopUpController() const;

public:
    ScriptComponentRef<Transform2D> m_popUpImage;

    float m_fadeInDuration = 0.25f;
    float m_fadeOutDuration = 0.25f;
};
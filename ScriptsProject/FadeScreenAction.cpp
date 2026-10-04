#include "pch.h"
#include "FadeScreenAction.h"

#include "ScreenFader.h"

IMPLEMENT_SCRIPT_FIELDS_INHERITED(FadeScreenAction, CameraTransitionStepAction,
    SERIALIZED_COMPONENT_REF(m_screenFaderObject, "Screen Fader Object", ComponentType::TRANSFORM),
    SERIALIZED_FLOAT(m_targetAlpha, "Target Alpha", 0.0f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_fadeDuration, "Fade Duration", 0.0f, 20.0f, 0.05f)
)

FadeScreenAction::FadeScreenAction(GameObject* owner)
    : CameraTransitionStepAction(owner)
{
}

void FadeScreenAction::executeAction(CameraTransitionController* controller, CameraTransitionStep* step)
{
    Transform* faderTransform = m_screenFaderObject.getReferencedComponent();
    if (!faderTransform)
    {
        Debug::warn("FadeScreenAction on '%s' has no Screen Fader Object assigned.", GameObjectAPI::getName(getOwner()));
        return;
    }

    GameObject* faderObject = ComponentAPI::getOwner(faderTransform);
    if (!faderObject)
    {
        return;
    }

    ScreenFader* screenFader = GameObjectAPI::findScript<ScreenFader>(faderObject);
    if (!screenFader)
    {
        Debug::warn("FadeScreenAction on '%s' could not find ScreenFader on the referenced object.", GameObjectAPI::getName(getOwner()));
        return;
    }

    screenFader->fadeTo(m_targetAlpha, m_fadeDuration);
}

IMPLEMENT_SCRIPT(FadeScreenAction)

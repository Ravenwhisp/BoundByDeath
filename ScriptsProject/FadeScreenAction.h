#pragma once

#include "ScriptAPI.h"
#include "CameraTransitionStepAction.h"

class Transform;

// Lanza un fundido a pantalla completa desde un plano de la cinemática, a través del
// ScreenFader de la escena. Para el fundido a negro del final del boss.
class FadeScreenAction : public CameraTransitionStepAction
{
    DECLARE_SCRIPT(FadeScreenAction)

public:
    explicit FadeScreenAction(GameObject* owner);

    FieldList getExposedFields() const override;

private:
    void executeAction(CameraTransitionController* controller, CameraTransitionStep* step) override;

public:
    ComponentRef<Transform> m_screenFaderObject;

    float m_targetAlpha = 1.0f;
    float m_fadeDuration = 1.5f;
};

#pragma once

#include "ScriptAPI.h"
#include "CameraTransitionStepAction.h"

class Transform;

// Dispara un bark desde un plano concreto de una cinematica. El momento y el retardo los
// gestiona la clase base; aqui solo se resuelve el BarkEvent y se lanza.
class BarkStepAction : public CameraTransitionStepAction
{
    DECLARE_SCRIPT(BarkStepAction)

public:
    explicit BarkStepAction(GameObject* owner);

    FieldList getExposedFields() const override;

private:
    void executeAction(CameraTransitionController* controller, CameraTransitionStep* step) override;

public:
    ComponentRef<Transform> m_barkObject;
    bool m_barkOnlyOnce = true;

private:
    bool m_hasPlayed = false;
};

#pragma once

#include "ScriptAPI.h"
#include "CameraTransitionStepAction.h"

class Transform;

// Lanza la transformación del boss (VFX del pilar de almas + cambio de modelo) desde un plano
// concreto de la cinemática. El cambio de modelo no ocurre aquí: lo programa el controller con
// su propia espera, para que caiga tapado por el pilar.
class AelorinPhaseSwapAction : public CameraTransitionStepAction
{
    DECLARE_SCRIPT(AelorinPhaseSwapAction)

public:
    explicit AelorinPhaseSwapAction(GameObject* owner);

    FieldList getExposedFields() const override;

private:
    void executeAction(CameraTransitionController* controller, CameraTransitionStep* step) override;

public:
    ComponentRef<Transform> m_bossTransform;
};

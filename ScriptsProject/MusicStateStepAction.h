#pragma once

#include "ScriptAPI.h"
#include "CameraTransitionStepAction.h"

// Cambia el estado de la musica desde un plano de una cinematica. Es la via limpia para
// que la fase 2 del jefe tenga su propio tema sin tocar su controller: el swap de modelo
// ya ocurre dentro de esta misma cinematica.
class MusicStateStepAction : public CameraTransitionStepAction
{
    DECLARE_SCRIPT(MusicStateStepAction)

public:
    explicit MusicStateStepAction(GameObject* owner);

    FieldList getExposedFields() const override;

private:
    void executeAction(CameraTransitionController* controller, CameraTransitionStep* step) override;

public:
    int m_musicState = 0;
};

#pragma once

#include "ScriptAPI.h"
#include "GameplayEventAction.h"

class GameplayEventTrigger;

// Arranca el encuentro del boss final cuando AMBOS jugadores entran en la arena. Va en el
// MISMO GameObject que un GameplayEventTrigger (con su collider TRIGGER), igual que
// CombatAreaEvent o MusicStateEvent.
//
// El GameObject del trigger NO lleva CameraTransitionEvent: la cinemática vive en su propio
// GameObject [CT] y la lanza el AelorinBossController a través de su ref Encounter Cinematic.
// Así hay un único camino de código y no se dispara dos veces.
//
// No es persistente a propósito: si lo fuese, al recargar un checkpoint el trigger se daría
// por disparado y el boss se quedaría quieto para siempre.
class AelorinEncounterEvent : public GameplayEventAction
{
    DECLARE_SCRIPT(AelorinEncounterEvent)

public:
    explicit AelorinEncounterEvent(GameObject* owner);

    void executeEvent(GameplayEventTrigger* trigger) override;

    FieldList getExposedFields() const override;

public:
    ComponentRef<Transform> m_bossTransform;
};

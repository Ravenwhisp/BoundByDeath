#pragma once

#include "ScriptAPI.h"
#include <unordered_set>

// Zona interna de música: cuando AMBOS jugadores entran en el área (AABB XZ),
// pide al MusicManager cambiar de State (p.ej. Level1_Upper -> Level1_Chapel).
// Mismo patrón de detección que LightZoneTrigger.
//
// El cambio de música SIEMPRE pasa por el MusicManager; este trigger solo decide
// CUÁNDO y a QUÉ estado. Sirve para Capilla (Lvl1) y Ascensor (Lvl2).
class MusicZoneTrigger : public Script
{
    DECLARE_SCRIPT(MusicZoneTrigger)

public:
    explicit MusicZoneTrigger(GameObject* owner);

    void Start() override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

    // Tamaño del área (centrada en la posición global del owner).
    float m_xWidth = 4.0f;
    float m_zWidth = 4.0f;

    // Índices en kMusicStateNames (mismo dropdown que el MusicManager).
    int  m_targetState  = 0;     // estado al entrar ambos jugadores
    int  m_stateOnExit  = 0;     // estado al salir (solo si m_changeOnExit)
    bool m_changeOnExit = false; // ¿revertir música al salir de la zona?
    bool m_triggerOnce  = false; // ¿solo la primera vez que entran?

private:
    bool containsPoint(const Vector3& center, const Vector3& point) const;

    std::unordered_set<GameObject*> m_playersInside;
    bool m_bothWereInside = false;
    bool m_hasTriggered   = false;
};

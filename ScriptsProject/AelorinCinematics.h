#pragma once

#include "ScriptAPI.h"

#include <string>
#include <vector>

class AelorinBossController;
class AelorinVFX;
class CameraShake;
class CameraTransitionController;
class CameraFollow;

// Orquesta las tres cinemáticas del boss final: entrada, cambio de fase y derrota.
//
// Vive como subscript en el GameObject del boss y NO modifica al AelorinBossController: lee su
// estado público y llama a beginPhase2() cuando toca. Toda la lógica de cinemática está aquí
// para no tocar scripts que ya hacen otras cosas.
//
// Quién la llama:
//   AelorinEncounterEvent   -> requestEncounterStart()      (ambos jugadores entran en la arena)
//   AelorinPhaseTransitionState -> startPhaseTransition()   (al entrar en el estado)
//   AelorinPhaseSwapAction  -> performPhaseTransformation() (desde el plano elegido)
//   La derrota se detecta sola, mirando isDead() del controller.
class AelorinCinematics : public Script
{
    DECLARE_SCRIPT(AelorinCinematics)

public:
    explicit AelorinCinematics(GameObject* owner);

    void Start() override;
    void Update() override;

    FieldList getExposedFields() const override;

    void requestEncounterStart();

    // Devuelve false si no hay cinemática asignada, para que el state se quede con su
    // comportamiento de siempre.
    bool startPhaseTransition();

    void performPhaseTeleportToCenter();
    bool isPhaseTeleportFinished() const { return m_phaseTeleportFinished; }

    bool isPlaying() const { return m_encounterPlaying || m_phasePlaying || m_defeatPlaying; }

public:
    ComponentRef<Transform> m_encounterCinematic;
    ComponentRef<Transform> m_healthBarRoot;

    // Durante el combate la cámara deja de seguir a los jugadores y se queda fija sobre la
    // arena. Se consigue apuntando los dos targets del CameraFollow a este ancla: el midpoint
    // pasa a ser siempre el mismo punto. Se aplica al arrancar el encuentro, no antes.
    // UI de mundo que estorba durante las cinematicas (indicadores de target, etc).
    std::vector<ComponentRef<Transform>> m_hideDuringCinematics;

    ComponentRef<Transform> m_arenaCameraAnchor;
    Vector3 m_arenaCameraOffset = Vector3(31.34f, 48.22f, -31.34f);

    // El plano de los jugadores se recoloca en runtime sobre su punto medio, porque no se sabe
    // por donde van a entrar.
    Vector3 m_playersShotOffset = Vector3(5.85f, 9.0f, -5.85f);

    float m_encounterRoarDelay = 4.0f;
    bool m_setMusicOnEncounter = true;
    bool m_debugReplayEncounter = false;

    ComponentRef<Transform> m_phaseTransitionCinematic;
    float m_phaseSwapDelay = 1.2f;

    // El cambio de fase puede pillar al boss en cualquier punto de la arena, asi que su primer
    // plano tambien se recoloca en runtime.
    Vector3 m_phaseShotOffset = Vector3(4.29f, 6.6f, -4.29f);

    // Plano desde el que se ve a la vez a Aelorin y el centro, antes de que se teletransporte.
    // De este vector solo se usa la DIRECCION: la distancia se calcula con la separacion real
    // entre el boss y el centro, para que los dos entren siempre en cuadro.
    Vector3 m_phaseWideShotOffset = Vector3(10.14f, 15.6f, -10.14f);
    float m_phaseWideBaseDistance = 24.0f;
    float m_phaseWideDistancePerUnit = 1.5f;
    float m_phaseWideMaxDistance = 40.0f;

    ComponentRef<Transform> m_combatCameraAnchor;
    Vector3 m_combatCameraOffset = Vector3(35.0f, 54.0f, -35.0f);
    Vector3 m_combatCameraCloseOffset = Vector3(12.0f, 18.0f, -12.0f);

    Vector3 m_combatCameraFarOffset = Vector3(20.0f, 30.0f, -20.0f);

    float m_combatCameraZoomStartDistance = 8.0f;
    float m_combatCameraZoomMaxDistance = 22.0f;

    Vector3 m_combatCurrentFocus = Vector3::Zero;
    Vector3 m_combatCurrentOffset = Vector3(12.0f, 18.0f, -12.0f);

    float m_combatFocusSmoothSpeed = 3.0f;
    float m_combatZoomSmoothSpeed = 2.0f;

    bool m_combatCameraInitialized = false;

    ComponentRef<Transform> m_defeatCinematic;
    float m_defeatDelay = 0.0f;
    Vector3 m_defeatCamOffset = Vector3(7.0f, 10.0f, -7.0f);
    std::string m_sceneAfterDefeat = "";

private:
    void updateEncounter(float dt);
    void updatePhaseTransition(float dt);
    void updateDefeat(float dt);

    bool playCinematic(const ComponentRef<Transform>& cinematicRef);
    bool isTransitionRunning() const;

    void onEncounterFinished();

    void updateCombatCamera();

    // Mueve CameraPoints/<pointName> de una cinematica a target + offset, en global.
    void positionShot(const ComponentRef<Transform>& cinematicRef, const char* pointName,
                      const Vector3& target, const Vector3& offset);
    Vector3 getBossPosition() const;
    void onDefeatFinished();

    void showHealthBar(bool show);
    void setCinematicUIHidden(bool hidden);
    void applyArenaCamera();

private:
    AelorinBossController* m_controller = nullptr;
    AelorinVFX* m_vfx = nullptr;
    CameraShake* m_cameraShake = nullptr;
    CameraTransitionController* m_cameraTransition = nullptr;
    CameraFollow* m_cameraFollow = nullptr;

    bool m_initialHideDone = false;

    bool m_encounterRequested = false;
    bool m_encounterPlaying = false;
    bool m_encounterFinished = false;
    float m_encounterTimer = 0.0f;
    bool m_encounterRoarPlayed = false;

    bool m_phasePlaying = false;
    float m_phaseTimer = 0.0f;
    bool m_phaseSwapPending = false;
    float m_phaseSwapTimer = 0.0f;
    bool m_phaseTeleportFinished = false;

    bool m_defeatHandled = false;
    bool m_defeatPlaying = false;
    float m_defeatTimer = 0.0f;
    bool m_defeatPending = false;
    float m_defeatPendingTimer = 0.0f;
};

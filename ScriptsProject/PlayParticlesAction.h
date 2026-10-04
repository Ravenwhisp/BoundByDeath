#pragma once

#include "ScriptAPI.h"
#include "CameraTransitionStepAction.h"
#include "ParticleLifecycle.h"

class Transform;

// Lanza un prefab de partículas desde un plano de la cinemática, en la posición de un objeto
// (más un offset). Sirve para efectos que tienen que salir donde esté el objeto en ese momento,
// como el boss, que se mueve por la arena.
class PlayParticlesAction : public CameraTransitionStepAction
{
    DECLARE_SCRIPT(PlayParticlesAction)

public:
    explicit PlayParticlesAction(GameObject* owner);

    void Update() override;
    void OnGameStop() override;

    FieldList getExposedFields() const override;

private:
    void executeAction(CameraTransitionController* controller, CameraTransitionStep* step) override;

public:
    PrefabRef m_particlePrefab;
    ComponentRef<Transform> m_spawnAt;

    Vector3 m_spawnOffset = Vector3(0.0f, 0.0f, 0.0f);
    float m_lifetime = 3.0f;

private:
    ParticleLifecycle::TimedParticleTracker m_spawned;
};

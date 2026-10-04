#pragma once

#include "ScriptAPI.h"
#include "ParticleLifecycle.h"

#include <vector>

// VFX del boss final. Mismo patrón que ArthurParticles: refs a prefabs de partículas y
// esperas serializadas, con ParticleLifecycle gestionando instancias y limpieza.
//
// Prefabs que encajan con cada ref (Assets/Prefabs/Particles/VFXRemake/Enemies/BossFight/):
//   Soul Pillar       -> Summoning/PS_BossSummonEnemyColumn   (columna vertical, loop)
//   Souls Aura        -> AuraPhase2/PS_BossAuraSouls           (almas cian a rojo, loop)
//   Turning Embers 1  -> PhaseTransformation/PS_BossTurningEmbers1
//   Turning Embers 2  -> PhaseTransformation/PS_BossTurningEmbers2
//   Turning Shockwave -> PhaseTransformation/PS_BossTurningShockwave1 y 2 (one shot)
//   Phase 2 Aura      -> PS_BossPhase2
class AelorinParticles final : public Script
{
    DECLARE_SCRIPT(AelorinParticles)

public:
    explicit AelorinParticles(GameObject* owner);

    void Start() override;
    void Update() override;
    void OnGameStop() override;

    FieldList getExposedFields() const override;

    // Secuencia completa de la transformación: el pilar de almas y las embers arrancan ya,
    // los shockwaves entran con sus esperas. El pilar es lo que tapa el cambio de modelo.
    void playPhaseTransformation();

    void startSoulPillar();
    void stopSoulPillar();

    void startPhase2Aura();
    void stopPhase2Aura();

    void releaseRuntimeParticles();

public:
    PrefabRef m_soulPillarPrefab;
    PrefabRef m_soulsAuraPrefab;
    PrefabRef m_turningEmbers1Prefab;
    PrefabRef m_turningEmbers2Prefab;
    PrefabRef m_turningShockwave1Prefab;
    PrefabRef m_turningShockwave2Prefab;
    PrefabRef m_phase2AuraPrefab;

    // El prefab de la columna emite una sola particula a la vez, asi que por si solo no es un
    // cilindro. Se instancia varias veces en anillo para formarlo.
    int m_soulPillarCount = 10;
    float m_soulPillarRadius = 1.6f;

    Vector3 m_soulPillarScale = Vector3(1.0f, 1.0f, 1.0f);
    Vector3 m_soulPillarOffset = Vector3(0.0f, 0.0f, 0.0f);

    bool m_debugPlayTransformation = false; // Debug: dispara la secuencia desde el inspector

    float m_shockwave1Delay = 1.0f;
    float m_shockwave2Delay = 1.3f;
    float m_soulPillarDuration = 3.5f;
    float m_embersDuration = 4.0f;

private:
    struct TimedEffect
    {
        enum class Type
        {
            Shockwave1,
            Shockwave2,
            StopPillar
        };

        Type type = Type::Shockwave1;
        float timer = 0.0f;
    };

    void scheduleEffect(TimedEffect::Type type, float delay);
    void processTimedEffects(float deltaTime);

    void spawnShockwave(const PrefabRef& prefab);

    Vector3 getBossPosition() const;

    void applyPillarTransform();

private:
    Transform* m_ownerTransform = nullptr;

    std::vector<GameObject*> m_soulPillarInstances;
    GameObject* m_soulsAuraInstance = nullptr;
    GameObject* m_turningEmbers1Instance = nullptr;
    GameObject* m_turningEmbers2Instance = nullptr;
    GameObject* m_phase2AuraInstance = nullptr;

    bool m_phase2AuraActive = false;

    std::vector<TimedEffect> m_timedEffects;
    ParticleLifecycle::TimedParticleTracker m_timedOneShots;
};

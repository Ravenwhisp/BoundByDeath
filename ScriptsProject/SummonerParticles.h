#pragma once

#include "ScriptAPI.h"

class SummonerParticles final : public Script
{
    DECLARE_SCRIPT(SummonerParticles)

public:
    explicit SummonerParticles(GameObject* owner);

    void Update() override;

    FieldList getExposedFields() const override;

    void playSummonParticle(const Vector3& position);
    void playTeleportParticle(const Vector3& position);

private:
    struct TimedParticle
    {
        GameObject* gameObject = nullptr;
        float timer = 0.0f;
    };

    void spawnTimedParticle(const PrefabRef& prefab, const Vector3& position, const Vector3& rotation, float lifetime);
    void updateTimedParticles(float deltaTime);

    Vector3 getOwnerRotation() const;

private:
    PrefabRef m_summonParticlePrefab;
    PrefabRef m_teleportParticlePrefab;

    float m_summonParticleLifetime = 2.0f;
    float m_teleportParticleLifetime = 1.0f;

    float m_summonYOffset = 0.0f;

    std::vector<TimedParticle> m_timedParticles;
};
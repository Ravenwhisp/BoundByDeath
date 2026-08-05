#pragma once

#include "ScriptAPI.h"

class SkeletonParticles final : public Script
{
    DECLARE_SCRIPT(SkeletonParticles)

public:
    explicit SkeletonParticles(GameObject* owner);

    void Start() override;
    void Update() override;

    FieldList getExposedFields() const override;

    void startReviveParticle();
    void stopReviveParticle();

    void playShockwaveParticle();

private:
    Vector3 getReviveParticlePosition() const;
    Vector3 getShockwaveParticlePosition() const;
    Vector3 getOwnerRotation() const;

    void updateReviveParticle();
    void removeShockwaveParticle();

private:
    PrefabRef m_reviveParticlePrefab;
    PrefabRef m_shockwaveParticlePrefab;

    float m_reviveYOffset = 0.0f;
    float m_reviveForwardOffset = 0.0f;

    float m_shockwaveYOffset = 0.05f;
    float m_shockwaveForwardOffset = 0.75f;
    float m_shockwaveLifetime = 1.0f;

    Transform* m_ownerTransform = nullptr;

    GameObject* m_reviveParticle = nullptr;
    Transform* m_reviveParticleTransform = nullptr;

    GameObject* m_shockwaveParticle = nullptr;
    float m_shockwaveTimer = 0.0f;
};
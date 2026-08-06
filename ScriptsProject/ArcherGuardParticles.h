#pragma once
#include "ScriptAPI.h"
#include <vector>

class ArcherGuardParticles : public Script
{
    DECLARE_SCRIPT(ArcherGuardParticles)
public:
    explicit ArcherGuardParticles(GameObject* owner);
    void Start() override;
    void Update() override;

    FieldList getExposedFields() const override;

    // Basic attack trail — called by ArcherArrowShooter
    void spawnBasicAttackTrail(const Vector3& pos);
    void syncBasicAttackTrail(const Vector3& pos, const Vector3& eulerDeg);
    void stopBasicAttackTrail();

    // Barrage — called by ArcherArrowBarrageState
    void startBarrageFloorParticle(const Vector3& position);
    void stopBarrageFloorParticle();

    void playBarrageImpactParticle(const Vector3& position);

    // Charge/somersault — called by ArcherSomersaultState
    void startChargeParticle();
    void updateChargeParticle();
    void stopChargeParticle();

public:    
    PrefabRef m_trailPrefab;
    PrefabRef m_barrageFloorPrefab;
    PrefabRef m_barrageImpactPrefab;
    PrefabRef m_somersaultPrefab;

    float m_barrageFloorYOffset = 0.05f;
    float m_barrageImpactYOffset = 0.05f;
    float m_barrageImpactLifetime = 1.0f;

private:
    GameObject* m_trailGO  = nullptr;
    GameObject* m_barrageFloorParticle = nullptr;
    GameObject* m_barrageImpactParticle = nullptr;

    float m_barrageImpactTimer = 0.0f;

    GameObject* m_somersaultParticle = nullptr;
    Transform* m_somersaultParticleTransform = nullptr;
};

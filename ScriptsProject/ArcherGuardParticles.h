#pragma once
#include "ScriptAPI.h"
#include <string>
#include <vector>

class ArcherAttackConfig;
class RangedEnemyController;
class AnimationComponent;

class ArcherGuardParticles : public Script
{
    DECLARE_SCRIPT(ArcherGuardParticles)
public:
    explicit ArcherGuardParticles(GameObject* owner);
    void Start()  override;
    void Update() override;
    ScriptFieldList getExposedFields() const override;

    std::string m_volleyPrefab  = "Assets/Prefabs/Particles/Archer/ArcherGuardVolley.prefab";
    std::string m_chargePrefab  = "Assets/Prefabs/Particles/Archer/ArcherGuardCharge.prefab";
    std::string m_arrowPrefab   = "Assets/Prefabs/Projectiles/ArcherArrow.prefab";

private:
    void updateBarrage(const char* state);
    void updateCharge(const char* state);

private:
    ArcherAttackConfig*    m_config     = nullptr;
    RangedEnemyController* m_controller = nullptr;
    AnimationComponent*    m_animation  = nullptr;

    // Barrage
    bool                   m_inBarrage          = false;
    float                  m_barrageTimer       = 0.0f;
    bool                   m_barrageImpactFired = false;
    bool                   m_barrageArrowsFired = false;
    Vector3                m_barrageImpactPos   = Vector3(0.0f, 0.0f, 0.0f);
    std::vector<GameObject*> m_barrageArrows;

    // Charge
    bool        m_inCharge         = false;
    GameObject* m_chargeParticleGO = nullptr;
};

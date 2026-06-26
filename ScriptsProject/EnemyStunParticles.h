#pragma once
#include "ScriptAPI.h"
#include <string>

class AnimationComponent;

class EnemyStunParticles : public Script
{
    DECLARE_SCRIPT(EnemyStunParticles)
public:
    explicit EnemyStunParticles(GameObject* owner);
    void Start()  override;
    void Update() override;
    ScriptFieldList getExposedFields() const override;

    std::string m_stunPrefab   = "Assets/Prefabs/Particles/Stun/StunSoul.prefab";
    float       m_heightOffset = 2.0f;

private:
    AnimationComponent* m_animation    = nullptr;
    bool                m_inStun       = false;
    GameObject*         m_stunParticle = nullptr;
};

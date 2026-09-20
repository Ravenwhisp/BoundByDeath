#pragma once

#include "ScriptAPI.h"
#include <vector>

struct ManagedParticle
{
    GameObject* gameObject           = nullptr;
    bool        deactivatedByManager = false;
};

class ParticleManager : public Script
{
    DECLARE_SCRIPT(ParticleManager)

public:
    explicit ParticleManager(GameObject* owner);

    void Start() override;
    void Update() override;
    void OnGameStop() override;

    void drawGizmo() override;

    FieldList getExposedFields() const override;

    // Explicit registration of dedicated VFX roots: objects whose only purpose
    // is holding particle systems. The manager never owns nor destroys them;
    // it only toggles their active state based on distance to the players.
    static void registerVfxRoot(GameObject* root);
    static void unregisterVfxRoot(GameObject* root);

public:
    float m_activationDistance   = 50.0f;
    float m_deactivationDistance = 60.0f;
    float m_checkIntervalSeconds = 1.0f;
    // When enabled, every particle system present in the scene at Start is
    // registered automatically, on top of the explicitly registered roots.
    bool m_manageAllParticles    = false;

private:
    bool registerRoot(GameObject* root);
    void adoptPendingRegistrations();
    void scanAndRegisterAll();
    bool isUnderManagedRoot(GameObject* obj) const;
    void pruneInvalidEntries();
    void updateActivity();
    bool isValidVfxRoot(GameObject* root) const;

    static ParticleManager*         s_instance;
    static std::vector<GameObject*> s_pendingRegistrations;

    float                        m_timer = 0.0f;
    std::vector<ManagedParticle> m_managedParticles;
};

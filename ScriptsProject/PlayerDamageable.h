#pragma once

#include "Damageable.h"
#include "HapticEffectDefinition.h"

class PlayerAnimationController;

class PlayerDamageable : public Damageable
{
    DECLARE_SCRIPT(PlayerDamageable)

public:
    explicit PlayerDamageable(GameObject* owner);

    void Start() override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

    float m_heartbeatThreshold = 0.5f;
    float m_hapticIntensity = 1.0f;

protected:
    void onDamaged(float amount) override;
    void onHpDepleted() override;
    void onDeath() override;
    void onRevive() override;

private:
    PlayerAnimationController* m_playerAnimationController = nullptr;

    void fireLub();

    float m_dubTimer = -1.0f;
    float m_lubTimer = -1.0f; 
    float m_dubScale = 0.0f;
    bool  m_dyingBeat = false;
};
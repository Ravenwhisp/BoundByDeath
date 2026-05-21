#pragma once

#include "ScriptAPI.h"

class ArcherAttackConfig : public Script
{
    DECLARE_SCRIPT(ArcherAttackConfig)

public:
    explicit ArcherAttackConfig(GameObject* owner);

    ScriptFieldList getExposedFields() const override;

public:
    // Basic attack
    float m_basicAttackRange = 4.0f;
    float m_basicAttackDamage = 10.0f;
    float m_basicAttackWindupTime = 0.35f;
    float m_basicAttackTotalDuration = 0.8f;
    float m_basicAttackCooldown = 1.2f;

    // Arrow Barrage
    float m_arrowBarrageDamage = 15.0f;
    float m_arrowBarrageRadius = 2.5f;
    float m_arrowBarrageCastTime = 0.8f;
    float m_arrowBarrageTotalDuration = 1.2f;
    float m_arrowBarrageCooldown = 5.0f;

    // Somersault
    float m_somersaultTriggerRange = 2.0f;
    float m_somersaultDistance = 4.0f;
    float m_somersaultDuration = 0.35f;
    float m_somersaultCooldown = 9.0f;
};
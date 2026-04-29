#pragma once

#include "ScriptAPI.h"

class HealthPickup : public Script
{
    DECLARE_SCRIPT(HealthPickup)

public:
    explicit HealthPickup(GameObject* owner);

    void Start()  override;
    void Update() override;
    void OnTriggerEnter(GameObject* player) override;

    ScriptFieldList getExposedFields() const override;

private:
    void idleAnimation();

public:

    bool m_collected = false;
    float m_healAmount   = 10.0f;
    float m_pickupRadius = 1.5f;

    Vector3 m_startPosition = Vector3::Zero;

    float m_idleTimer = 0.0f;
    float m_idleSpeed = 0.2f;
    float m_horizontalAmplitude = 0.1f;
    float m_verticalAmplitude = 0.2f;
};

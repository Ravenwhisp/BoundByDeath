#pragma once

#include "ScriptAPI.h"

class HealthPickup : public Script
{
    DECLARE_SCRIPT(HealthPickup)

public:
    explicit HealthPickup(GameObject* owner);

    void Start()  override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

public:
    float m_healAmount   = 10.0f;
    float m_pickupRadius = 1.5f;
};

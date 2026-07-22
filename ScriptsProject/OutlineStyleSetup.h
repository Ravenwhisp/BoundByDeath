#pragma once

#include "ScriptAPI.h"

class OutlineStyleSetup : public Script
{
    DECLARE_SCRIPT(OutlineStyleSetup)

public:
    explicit OutlineStyleSetup(GameObject* owner);

    void Start() override;

    FieldList getExposedFields() const override;

public:
    bool  m_enableOutline = true;

    float m_intensity = 1.0f;
    float m_thickness = 0.5f;
    float m_threshold = 0.001f;
    float m_wobble = 0.0f;
    float m_breakup = 0.0f;

    Vector3 m_color = Vector3(0.02f, 0.02f, 0.02f);
};

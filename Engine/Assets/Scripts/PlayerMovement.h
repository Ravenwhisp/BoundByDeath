#pragma once

#include "ScriptAPI.h"

class PlayerMovement : public Script
{
    DECLARE_SCRIPT(PlayerMovement)

public:
    explicit PlayerMovement(GameObject* owner);

    void Start() override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

    void applyTranslation(GameObject* owner, const Vector3& direction, float dt, bool shiftHeld) const;

public:
    float m_moveSpeed = 3.5f;
    float m_shiftMultiplier = 2.0f;

    bool m_constrainToNavMesh = true;
    Vector3 m_navExtents = Vector3(2.0f, 4.0f, 2.0f);

};
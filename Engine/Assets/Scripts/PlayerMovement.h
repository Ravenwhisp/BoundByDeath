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

	inline void normalMovement(GameObject* owner, const Vector3& timeStep, bool shiftHeld) const
    {
        float speed = shiftHeld ? (m_moveSpeed * m_shiftMultiplier) : m_moveSpeed;
        moveInternal(owner, timeStep * speed);
    }
    inline void dashMovement(GameObject* owner, const Vector3& displacement) const { moveInternal(owner, displacement); }

public:
    float m_moveSpeed = 3.5f;
    float m_shiftMultiplier = 2.0f;

    bool m_constrainToNavMesh = true;
    Vector3 m_navExtents = Vector3(2.0f, 4.0f, 2.0f);

private:
	void moveInternal(GameObject* owner, const Vector3& desiredPos) const;
    void applyTranslation(Transform* transform, const Vector3& currentPos, const Vector3& desiredPos) const;

};
#pragma once

#include "ScriptAPI.h"

class ArrowPool;

class LyrielArrowProjectile : public Script
{
    DECLARE_SCRIPT(LyrielArrowProjectile)

public:
    explicit LyrielArrowProjectile(GameObject* owner);

    void Update() override;

    ScriptFieldList getExposedFields() const override;

    void launch(const Vector3& start_position, const Vector3& direction, float speed);
    void resetProjectile();
    void returnToPool();

    bool isInUse() const;
    void setPool(ArrowPool* pool);

public:
    float m_maxLifetime = 2.0f;

private:
    ArrowPool* m_pool = nullptr;

    bool m_inUse = false;
    Vector3 m_direction = Vector3::Zero;
    float m_speed = 0.0f;
    float m_lifeTimer = 0.0f;
};
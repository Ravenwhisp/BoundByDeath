#pragma once

#include "ScriptAPI.h"

class ArthurAttackConfig;

class ArthurAttackDebugDraw : public Script
{
    DECLARE_SCRIPT(ArthurAttackDebugDraw)

public:
    explicit ArthurAttackDebugDraw(GameObject* owner);

    void Start() override;
    void drawGizmo() override;

    ScriptFieldList getExposedFields() const override;

public:
    bool m_debugEnabled = true;
    bool m_drawHeavySwipe = true;
    bool m_drawEarthHammer = true;

    float m_heightOffset = 0.15f;

private:
    ArthurAttackConfig* m_attackConfig = nullptr;

private:
    void drawHeavySwipeCone() const;
    void drawEarthHammerRadius() const;

    Vector3 rotateAroundY(const Vector3& vector, float radians) const;
};
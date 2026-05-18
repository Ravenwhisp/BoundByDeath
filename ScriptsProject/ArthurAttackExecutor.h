#pragma once

#include "ScriptAPI.h"

class ArthurDetectionAggro;

class ArthurAttackExecutor : public Script
{
    DECLARE_SCRIPT(ArthurAttackExecutor)

public:
    explicit ArthurAttackExecutor(GameObject* owner);

    void Start() override;

    void applyDamageInRadius(const Vector3& center, float radius, float damage, const char* sourceName);
    void applyDamageInCone(const Vector3& center, const Vector3& direction, float range, float halfAngleDegrees, float damage, const char* sourceName);

private:
    void tryDamageTargetInRadius(Transform* targetTransform, const Vector3& center, float radius, float damage, const char* sourceName);
    void tryDamageTargetInCone(Transform* targetTransform, const Vector3& center, const Vector3& direction, float range, float halfAngleDegrees, float damage, const char* sourceName);

    void applyDamageToTarget(Transform* targetTransform, float damage, const char* sourceName);

private:
    ArthurDetectionAggro* m_arthurDetectionAggro = nullptr;
};
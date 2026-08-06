#pragma once

#include "ScriptAPI.h"

class AelorinDetectionAggro;

class AelorinAttackExecutor : public Script
{
    DECLARE_SCRIPT(AelorinAttackExecutor)

public:
    explicit AelorinAttackExecutor(GameObject* owner);

    void Start() override;

    void applyDamageInRadius(const Vector3& center, float radius, float damage, const char* sourceName);
    bool tryDamageTargetInRadius(Transform* targetTransform, const Vector3& center, float radius, float damage, const char* sourceName);

private:
    bool applyDamageToTarget(Transform* targetTransform, float damage, const char* sourceName);

private:
    AelorinDetectionAggro* m_detectionAggro = nullptr;
};
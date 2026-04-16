#pragma once

#include "AbilityBase.h"

class LyrielCharacter;
class PlayerRotation;

class LyrielAbilityBase : public AbilityBase
{
public:
    explicit LyrielAbilityBase(GameObject* owner);

    void Start() override;
    void Update() override;

protected:
    Transform* findArrowSpawnTransform() const;
    void faceDirection(const Vector3& direction);

    Vector3 getFallbackFacingDirection() const;

    void updateAttackFacing();
    void updateAttackStateTimer();
    void beginAttackLock(const Vector3& facingDirection, float lockDuration);

protected:
    LyrielCharacter* m_lyriel = nullptr;

    float m_attackStateTimer = 0.0f;
    Vector3 m_attackFacingDirection = Vector3::Zero;
};
#pragma once

#include "AbilityBase.h"

class DeathCharacter;
class PlayerState;
class PlayerAnimationController;
class PlayerRotation;

class DeathBasicAttack : public AbilityBase
{
    DECLARE_SCRIPT(DeathBasicAttack)

public:
    explicit DeathBasicAttack(GameObject* owner);

    void Start()      override;
    void Update()     override;
    void drawGizmo()  override;

    ScriptFieldList getExposedFields() const override;

    float m_attackLockDuration = 0.35f;

private:
    void faceTarget(GameObject* target);

    float m_attackLockTimer = 0.0f;

    DeathCharacter*            m_deathChar          = nullptr;
    GameObject*                m_attackFacingTarget = nullptr;
    PlayerState*               m_playerState        = nullptr;
    PlayerAnimationController* m_animController     = nullptr;
    PlayerRotation*            m_playerRotation     = nullptr;
};

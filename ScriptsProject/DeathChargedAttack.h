#pragma once

#include "AbilityBase.h"

class DeathCharacter;
class PlayerState;
class PlayerAnimationController;
class PlayerRotation;

class DeathChargedAttack : public AbilityBase
{
    DECLARE_SCRIPT(DeathChargedAttack)

public:
    explicit DeathChargedAttack(GameObject* owner);

    void Start()  override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

public:
    float m_minChargeTime             = 0.5f;
    float m_maxChargeTime             = 1.0f;
    float m_attackLockDuration        = 0.55f;
    float m_chargedAttackLockDuration = 0.8f;

private:
    void faceTarget(GameObject* target);

    float m_chargeTime      = 0.0f;
    float m_attackLockTimer = 0.0f;
    bool  m_quickPressOnly  = false;

    DeathCharacter*            m_deathChar          = nullptr;
    GameObject*                m_attackFacingTarget = nullptr;
    PlayerState*               m_playerState        = nullptr;
    PlayerAnimationController* m_animController     = nullptr;
    PlayerRotation*            m_playerRotation     = nullptr;
};

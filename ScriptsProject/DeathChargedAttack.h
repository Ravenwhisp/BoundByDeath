#pragma once

#include "AbilityBase.h"

class PlayerState;
class PlayerAnimationController;
class PlayerRotation;

// Death's heavy attack — R2 (RT) with two modes:
//
//   Quick press  (release before m_minChargeTime):
//     Deals m_chargedAttackDamage to all enemies in the attack arc.
//     Can be used up to twice consecutively in a combo (enforced by DeathCharacter).
//
//   Charged press (hold ≥ m_minChargeTime, release to fire):
//     Scales damage from 1× to 2× m_chargedAttackDamage based on how close
//     the charge is to m_maxChargeTime.
//     COMBO STARTER ONLY — ignored if already mid-combo (comboStep > 0).
//     After landing, the combo continues normally.
//
// isActive() reflects whether the trigger is currently held.
// Arc parameters (range, angle) live on DeathCharacter.
class DeathChargedAttack : public AbilityBase
{
    DECLARE_SCRIPT(DeathChargedAttack)

public:
    explicit DeathChargedAttack(GameObject* owner);

    void Start()  override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

public:
    // Minimum hold time (seconds) that distinguishes a hold from a quick press.
    float m_minChargeTime = 0.5f;

    // How long to lock input after the hit lands — tune to match the animation length.
    // Must be less than DeathCharacter::m_comboWindow.
    float m_attackLockDuration = 0.55f;

private:
    void faceTarget(GameObject* target);

    float m_chargeTime      = 0.0f;
    float m_attackLockTimer = 0.0f;

    GameObject*                m_attackFacingTarget = nullptr;
    PlayerState*               m_playerState        = nullptr;
    PlayerAnimationController* m_animController     = nullptr;
    PlayerRotation*            m_playerRotation     = nullptr;
};

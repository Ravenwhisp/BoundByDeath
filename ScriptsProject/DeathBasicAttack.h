#pragma once

#include "AbilityBase.h"

class PlayerState;
class PlayerAnimationController;
class PlayerRotation;

// Death's light attack — chains up to 3 hits (R1 / RB).
// Sibling script of DeathCharacter on the same GameObject.
//
// The ability stays active for m_attackLockDuration seconds after each hit,
// blocking canAct (and therefore the next hit) for that window.
// This gives the animation time to play before the next R1 is accepted.
//
// Combo state (step, consecutive R2 count, window timer) is shared with
// DeathChargedAttack and lives in DeathCharacter.
//
// Every hit deals damage to ALL enemies within the character's attack arc
// (m_arcRange, m_arcAngle on DeathCharacter).
class DeathBasicAttack : public AbilityBase
{
    DECLARE_SCRIPT(DeathBasicAttack)

public:
    explicit DeathBasicAttack(GameObject* owner);

    void Start()      override;
    void Update()     override;
    void drawGizmo()  override;

    ScriptFieldList getExposedFields() const override;

    // How long (seconds) to lock input after each hit — controls animation breathing room
    // and minimum gap between combo hits. Must be less than DeathCharacter::m_comboWindow.
    float m_attackLockDuration = 0.35f;

private:
    void faceTarget(GameObject* target);

    float m_attackLockTimer = 0.0f;

    GameObject*                m_attackFacingTarget = nullptr;
    PlayerState*               m_playerState        = nullptr;
    PlayerAnimationController* m_animController     = nullptr;
    PlayerRotation*            m_playerRotation     = nullptr;
};

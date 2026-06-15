#pragma once

#include "DeathAbilityBase.h"

class DeathUI;
class PlayerMovement;

class DeathBasicAttack : public DeathAbilityBase
{
    DECLARE_SCRIPT(DeathBasicAttack)

public:
    explicit DeathBasicAttack(GameObject* owner);

    void Start()      override;
    void Update()     override;
    void drawGizmo()  override;

protected:
    void onAttackWindowUpdate()   override;
    void onAttackWindowFinished() override;

	bool canStartSpecificAbility() const override;

    float getCooldown() const override;

private:
    void startAbility() override;
    void snapFaceTarget(GameObject* target);
    void faceTarget(GameObject* target);
    void closeGapToTarget(GameObject* target, float lockDuration);
	void dealDamageToTarget(GameObject* target) const;

    void updateUI() override;

private:
    DeathUI* m_deathUI = nullptr;
    PlayerMovement* m_playerMovement = nullptr;
    Vector3 m_closeGapVelocity = Vector3::Zero;

    GameObject* m_attackFacingTarget = nullptr;
};

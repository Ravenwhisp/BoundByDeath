#pragma once

#include "AbilityBase.h"

// Lyriel's charged attack — hold RT / R2 to enter aiming mode, release to fire (RT / R2).
// While aiming, the right stick (LookAxis) controls the shot direction.
// Sibling script of LyrielCharacter on the same GameObject.
// Reads m_chargedArrowDamage from LyrielCharacter via static_cast<LyrielCharacter*>(m_character).
//
// isActive() (from AbilityBase) reflects whether Lyriel is currently in aiming mode.
// Movement is optionally restricted while aiming (TODO: slow PlayerController speed).
class LyrielChargedAttack : public AbilityBase
{
    DECLARE_SCRIPT(LyrielChargedAttack)

public:
    explicit LyrielChargedAttack(GameObject* owner);

    void Start()  override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

    // To control behaviour INDEPENDENTLY of the input (and maybe use with AI)
    void startCharge();
    void releaseCharge();
    void releaseImmediately(); // to allow just a basic shot (probably not useful, since basic shot is on another script)

    bool isCharging() const { return m_isCharging; }

    float m_chargeTime = 2; // in seconds!

    std::string m_projectileToInstantiate = "";
    Vector3 m_spawnRelativePoint = Vector3(0.f, 0.f, 0.f); // respect to target direction

private:
    // Aim direction updated each frame from Input::getLookAxis while isActive().
    // Used to determine the arrow's flight direction on release.
    Vector2 m_aimDirection = { 0.0f, 0.0f };

    Transform* m_objectTransform; // the current object

    bool m_isCharging;
    float m_currentChargeTime;

    bool m_toReleaseCharge;
};

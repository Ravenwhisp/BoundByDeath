#pragma once

#include "CharacterBase.h"

// Personaje jugable Death — brawler cuerpo a cuerpo.
// Script hermano junto a: PlayerTargetController, DeathBasicAttack,
//                          DeathChargedAttack, DeathDash, DeathTaunt
//
// Las abilities de Death leen los stats de daño directamente de este script.
class DeathCharacter : public CharacterBase
{
    DECLARE_SCRIPT(DeathCharacter)

public:
    explicit DeathCharacter(GameObject* owner);

    void Start()  override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

    // --- Combo state (shared between DeathBasicAttack and DeathChargedAttack) ---
    //
    // comboStep: 0 = no combo, 1 = one hit landed, 2 = two hits landed.
    // After the third hit advanceCombo() resets it back to 0.
    //
    // consecutiveR2Count: how many R2s in a row have been used.
    // canUseR2InCombo() returns false when this reaches 2 (enforces max 2 R2s consecutively).
    int  getComboStep()        const { return m_comboStep; }
    bool canUseR2InCombo()     const { return m_consecutiveR2Count < 2; }

    // 1.0 = justo después de un hit, 0.0 = ventana expirada / sin combo activo.
    // Útil para el gizmo de la ventana de combo.
    float getComboFillRatio()  const
    {
        if (m_comboStep == 0 || m_comboWindow <= 0.0f) return 0.0f;
        const float r = 1.0f - (m_comboTimer / m_comboWindow);
        return r < 0.0f ? 0.0f : r;
    }

    // Call this when a hit lands.
    // isR2: true for heavy attacks, false for light attacks.
    void advanceCombo(bool isR2);

    // Resets the full combo state (step, R2 count, timer).
    void resetCombo();

    // Deals damage (and optionally stuns) all ENEMY-tagged objects within the
    // character's attack arc (m_arcRange, m_arcAngle). Called by sibling attack scripts.
    // stunDuration = 0 means no stun.
    void dealDamageInArc(float damage, float stunDuration = 0.0f) const;

protected:
    void onDamaged(float amount) override;
    void onDeath()               override;
    void onRevive()              override;

public:
    // --- Combat stats (read by sibling ability scripts) ---
    float m_basicAttackDamage    = 20.0f;
    float m_chargedAttackDamage  = 40.0f;
    float m_dashDistance         = 5.0f;
    float m_tauntDuration        = 2.0f;

    // --- Attack arc (shared by both attacks) ---
    float m_arcRange             = 2.5f;   // melee reach in world units
    float m_arcAngle             = 120.0f; // total cone angle in degrees

    // --- Stun durations (applied by R2 hits) ---
    float m_briefStunDuration    = 0.5f;   // quick R2
    float m_extendedStunDuration = 2.0f;   // max-charged R2

    // --- Charge parameters ---
    float m_maxChargeTime        = 2.0f;   // seconds to reach full charge
    float m_comboWindow          = 1.0f;   // seconds to chain the next hit

private:
    int   m_comboStep          = 0;
    int   m_consecutiveR2Count = 0;
    float m_comboTimer         = 0.0f;

    void tickCombo(float dt);
};

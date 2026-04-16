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

    void Start() override;

    ScriptFieldList getExposedFields() const override;

protected:
    // Reacciones propias de Death a eventos de HP
    void onDamaged(float amount) override;
    void onDeath() override;
    void onRevive() override;

public:
    // Stats leídos por las abilities hermanas en el mismo GO
    float m_basicAttackDamage = 20.0f;
    float m_chargedAttackDamage = 40.0f;
    float m_dashDistance = 5.0f;
    float m_tauntDuration = 2.0f;
};

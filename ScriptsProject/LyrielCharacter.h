#pragma once

#include "CharacterBase.h"

// Personaje jugable Lyriel — arquera a distancia.
// Script hermano junto a: PlayerTargetController, LyrielBasicAttack,
//                          LyrielChargedAttack, LyrielDash, LyrielArrowVolley
//
// Las abilities de Lyriel leen los stats de daño directamente de este script.
class LyrielCharacter : public CharacterBase
{
    DECLARE_SCRIPT(LyrielCharacter)

public:
    explicit LyrielCharacter(GameObject* owner);

    void Start() override;

    ScriptFieldList getExposedFields() const override;

protected:
    // Reacciones propias de Lyriel a eventos de HP
    void onDamaged(float amount) override;
    void onDeath() override;
    void onRevive() override;

public:
    // Stats leídos por las abilities hermanas en el mismo GO
    float m_basicArrowDamage = 15.0f;
    float m_chargedArrowDamage = 35.0f;
    float m_dashDistance = 6.0f;
    int   m_arrowVolleyCount = 5;
    float m_arrowVolleySpread = 30.0f;
};

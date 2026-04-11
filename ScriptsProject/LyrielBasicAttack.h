#pragma once

#include "AbilityBase.h"

// Lyriel's basic attack — fires a single arrow directly at the current target (RB / R1).
// Sibling script of LyrielCharacter on the same GameObject.
// Reads m_basicArrowDamage from LyrielCharacter via static_cast<LyrielCharacter*>(m_character).
//
// Instant ability: activates and deactivates within the same frame.
// A cooldown (m_cooldown) prevents spamming.
class LyrielBasicAttack : public AbilityBase
{
    DECLARE_SCRIPT(LyrielBasicAttack)

public:
    explicit LyrielBasicAttack(GameObject* owner);

    void Start()  override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;
};

#pragma once

#include "AbilityBase.h"

// Lyriel's arrow volley — fires N arrows spread in an arc around the current target (LT / L2).
// Sibling script of LyrielCharacter on the same GameObject.
// Reads m_arrowVolleyCount and m_arrowVolleySpread from LyrielCharacter
// via static_cast<LyrielCharacter*>(m_character).
//
// Instant ability: all arrows fire in the same frame.
// TODO: define whether arrows hit only enemies in the arc or the single target multiple times.
class LyrielArrowVolley : public AbilityBase
{
    DECLARE_SCRIPT(LyrielArrowVolley)

public:
    explicit LyrielArrowVolley(GameObject* owner);

    void Start()  override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;
};

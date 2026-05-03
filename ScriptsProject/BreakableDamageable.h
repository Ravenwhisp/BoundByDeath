#pragma once

#include "Damageable.h"

class BreakableDamageable : public Damageable
{
    DECLARE_SCRIPT(BreakableDamageable)

public:
    explicit BreakableDamageable(GameObject* owner);

protected:
    void onDeath() override;
};
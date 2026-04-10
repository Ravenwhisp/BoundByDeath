#pragma once

#include "Damageable.h"

class PlayerDamageable : public Damageable
{
    DECLARE_SCRIPT(PlayerDamageable)

public:
    explicit PlayerDamageable(GameObject* owner);

protected:
    void onHpDepleted() override;
};
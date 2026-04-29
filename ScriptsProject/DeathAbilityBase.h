#pragma once

#include "AbilityBase.h"

class DeathCharacter;

class DeathAbilityBase : public AbilityBase
{
public:
    explicit DeathAbilityBase(GameObject* owner);

    void Start() override;

protected:


protected:
    DeathCharacter* m_deathChar       = nullptr;
};

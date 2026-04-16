#pragma once

#include "CharacterBase.h"

class LyrielCharacter : public CharacterBase
{
    DECLARE_SCRIPT(LyrielCharacter)

public:
    explicit LyrielCharacter(GameObject* owner);

    void Start() override;
    ScriptFieldList getExposedFields() const override;

public:
    std::string m_arrowSpawnChildName = "ArrowSpawn";
};
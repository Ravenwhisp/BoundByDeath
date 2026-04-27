#pragma once

#include "ScriptAPI.h"

class PowerupCollectible : public Script
{
    DECLARE_SCRIPT(PowerupCollectible)

public:
    explicit PowerupCollectible(GameObject* owner);

    void OnTriggerEnter(GameObject* player) override;

    ScriptFieldList getExposedFields() const override;

private:
    bool canBeCollectedBy(GameObject* other) const;

private:
    enum PowerUpTarget
    {
        LYRIEL = 0,
        DEATH,
        BOTH
    };

    int m_targetCharacter = BOTH;
    bool m_collected = false;
};
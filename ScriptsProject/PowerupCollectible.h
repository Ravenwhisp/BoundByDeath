#pragma once

#include "ScriptAPI.h"

class PowerupCollectible : public Script
{
    DECLARE_SCRIPT(PowerupCollectible)

public:
    explicit PowerupCollectible(GameObject* owner);

    void Start() override;
    void OnTriggerEnter(GameObject* player) override;

private:
    bool m_collected = false;
};
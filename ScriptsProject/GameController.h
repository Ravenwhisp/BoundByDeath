#pragma once

#include "ScriptAPI.h"

// Central game-session controller.
// Lives on a dedicated "GameController" GameObject (Tag::DEFAULT).
// Sibling scripts like ReaperGauge are placed here so that both
// player characters can access session-level state without coupling
// to a specific player GameObject.

class GameController : public Script
{
    DECLARE_SCRIPT(GameController)

public:
    explicit GameController(GameObject* owner);

    void Start()  override;
    void Update() override;
};

#pragma once

#include "ScriptAPI.h"

class AnimationComponent;
class PlayerMovement;

enum class AnimState
{
    Idle,
    Move,
    Dash,
    Attack,
    Damaged,
    Death
};

class AnimationController : public Script
{
    DECLARE_SCRIPT(AnimationController)

public:
    explicit AnimationController(GameObject* owner);

    void Start() override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

    std::string m_idleStateName = "";
	std::string m_moveStateName = "";
	std::string m_dashStateName = "";
	std::string m_attackStateName = "";
	std::string m_damagedStateName = "";
	std::string m_deathStateName = "";

private:
	AnimationComponent* m_animationComponent = nullptr;
	PlayerMovement* m_playerMovement = nullptr;

    AnimState m_currentState = AnimState::Idle;

private:
    AnimationComponent* findAnimationComponent();
    PlayerMovement* findPlayerMovement();
};
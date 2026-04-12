#pragma once

#include "ScriptAPI.h"

class AnimationComponent;
class PlayerMovement;

enum class AnimState
{
    Idle,
    Move
};

class CharacterAnimation : public Script
{
    DECLARE_SCRIPT(CharacterAnimation)

public:
    explicit CharacterAnimation(GameObject* owner);

    void Start() override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

    std::string m_triggerIdleToMove = "";
	std::string m_triggerMoveToIdle = "";

private:
	AnimationComponent* m_animationComponent = nullptr;
	PlayerMovement* m_playerMovement = nullptr;

    AnimState m_currentState = AnimState::Idle;

private:
    AnimationComponent* findAnimationComponent();
    PlayerMovement* findPlayerMovement();
};
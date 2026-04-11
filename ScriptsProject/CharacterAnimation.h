#pragma once

#include "ScriptAPI.h"

class AnimationComponent;
class PlayerMovement;

class CharacterAnimation : public Script
{
    DECLARE_SCRIPT(CharacterAnimation)

public:
    explicit CharacterAnimation(GameObject* owner);

    void Start() override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;

    std::string m_moveStateName = "";
	std::string m_idleStateName = "";

private:
	AnimationComponent* m_animationComponent = nullptr;
	PlayerMovement* m_playerMovement = nullptr;

private:
    AnimationComponent* findAnimationComponent();
    PlayerMovement* findPlayerMovement();
};
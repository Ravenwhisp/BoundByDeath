#include "pch.h"
#include "AnimationController.h"

#include "PlayerMovement.h"

static const ScriptFieldInfo AnimationControllerFields[] =
{
	{ "Idle state name", ScriptFieldType::String, offsetof(AnimationController, m_idleStateName)  },
	{ "Move state name", ScriptFieldType::String, offsetof(AnimationController, m_moveStateName) },
	{ "Dash state name", ScriptFieldType::String, offsetof(AnimationController, m_dashStateName) },
	{ "Attack state name", ScriptFieldType::String, offsetof(AnimationController, m_attackStateName) },
	{ "Damaged state name", ScriptFieldType::String, offsetof(AnimationController, m_damagedStateName) },
	{ "Death state name", ScriptFieldType::String, offsetof(AnimationController, m_deathStateName) }
};

IMPLEMENT_SCRIPT_FIELDS(AnimationController, AnimationControllerFields)

AnimationController::AnimationController(GameObject* owner)
    : Script(owner)
{
}

void AnimationController::Start()
{
	m_animationComponent = findAnimationComponent();

	m_playerMovement = findPlayerMovement();
}

void AnimationController::Update()
{
	if (!m_animationComponent)
	{
		return;
	}

	const bool isMoving = m_playerMovement && m_playerMovement->m_isMoving;

	AnimState desiredState;
	if (isMoving)
	{
		desiredState = AnimState::Move;
	}
	/*
	* else if() add conditions from abilities
	* {
	* ]
	*/
	else
	{
		desiredState = AnimState::Idle;
	}

	if (desiredState == m_currentState)
	{
		return;
	}

	m_currentState = desiredState;

	const char* stateName = nullptr;

	switch (m_currentState)
	{
		case AnimState::Move: stateName = m_moveStateName.c_str(); break;
		case AnimState::Idle: stateName = m_idleStateName.c_str(); break;
		case AnimState::Dash: stateName = m_dashStateName.c_str(); break;
		case AnimState::Attack: stateName = m_attackStateName.c_str(); break;
		case AnimState::Damaged: stateName = m_damagedStateName.c_str(); break;
		case AnimState::Death: stateName = m_deathStateName.c_str(); break;
	}

	const bool played = AnimationAPI::playState(m_animationComponent, stateName, 0.25f);

	if (!played)
	{
		Debug::warn("CharacterAnimation on '%s' could not play state '%s'.",
			GameObjectAPI::getName(m_owner), stateName);
	}
	
}

AnimationComponent* AnimationController::findAnimationComponent()
{
	m_animationComponent = AnimationAPI::getAnimationComponent(m_owner);
	if (m_animationComponent)
	{
		return m_animationComponent;
	}
	Debug::warn("CharacterAnimation on '%s' could not find an AnimationComponent on the same GameObject.",
		GameObjectAPI::getName(m_owner));
	return nullptr;
}

PlayerMovement* AnimationController::findPlayerMovement()
{
	Script* movementScript = m_owner ? GameObjectAPI::getScript(m_owner, "PlayerMovement") : nullptr;
	if (movementScript)
	{
		return static_cast<PlayerMovement*>(movementScript);
	}
	Debug::warn("CharacterAnimation on '%s' could not find PlayerMovement on the same GameObject.",
		GameObjectAPI::getName(m_owner));
	return nullptr;
}

IMPLEMENT_SCRIPT(AnimationController)
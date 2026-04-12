#include "pch.h"
#include "CharacterAnimation.h"

#include "PlayerMovement.h"

static const ScriptFieldInfo CharacterAnimationFields[] =
{
	{ "\"Idle -> Move\" trigger name", ScriptFieldType::String, offsetof(CharacterAnimation, m_triggerIdleToMove)  },
	{ "\"Move -> Idle\" trigger name", ScriptFieldType::String, offsetof(CharacterAnimation, m_triggerMoveToIdle) }
};

IMPLEMENT_SCRIPT_FIELDS(CharacterAnimation, CharacterAnimationFields)

CharacterAnimation::CharacterAnimation(GameObject* owner)
    : Script(owner)
{
}

void CharacterAnimation::Start()
{
	m_animationComponent = findAnimationComponent();

	m_playerMovement = findPlayerMovement();
}

void CharacterAnimation::Update()
{
	if (!m_animationComponent)
	{
		return;
	}

	const bool isMoving = m_playerMovement && m_playerMovement->m_isMoving;

	AnimState desiredState = isMoving ? AnimState::Move : AnimState::Idle;

	if (desiredState == m_currentState)
	{
		return;
	}

	m_currentState = desiredState;

	const char* triggerName = nullptr;

	switch (m_currentState)
	{
		case AnimState::Move: triggerName = m_triggerIdleToMove.c_str(); break;
		case AnimState::Idle: triggerName = m_triggerMoveToIdle.c_str(); break;
	}

	const bool played = AnimationAPI::sendTrigger(m_animationComponent, triggerName);

	if (!played)
	{
		Debug::warn("CharacterAnimation on '%s' could not play trigger '%s'.",
			GameObjectAPI::getName(m_owner), triggerName);
	}
	
}

AnimationComponent* CharacterAnimation::findAnimationComponent()
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

PlayerMovement* CharacterAnimation::findPlayerMovement()
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

IMPLEMENT_SCRIPT(CharacterAnimation)
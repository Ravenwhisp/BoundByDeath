#include "pch.h"
#include "CharacterAnimation.h"

#include "PlayerMovement.h"

static const ScriptFieldInfo CharacterAnimationFields[] =
{
	{ "Idle State Name", ScriptFieldType::String, offsetof(CharacterAnimation, m_idleStateName)  },
	{ "Move State Name", ScriptFieldType::String, offsetof(CharacterAnimation, m_moveStateName) }
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

	if(m_playerMovement && m_playerMovement->m_isMoving)
	{
		const bool played = AnimationAPI::playState(m_animationComponent, m_moveStateName.c_str(), 0.0f);
		if(!played)
			Debug::warn("CharacterAnimation on '%s' could not play move state '%s'. Check if the state name is correct and exists in the AnimationComponent.",
				GameObjectAPI::getName(m_owner), m_moveStateName.c_str());
		return;
	}

	const bool played = AnimationAPI::playState(m_animationComponent, m_idleStateName.c_str(), 0.0f);
	if(!played)
		Debug::warn("CharacterAnimation on '%s' could not play idle state '%s'. Check if the state name is correct and exists in the AnimationComponent.",
			GameObjectAPI::getName(m_owner), m_idleStateName.c_str());
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
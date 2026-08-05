#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

#include "AelorinBossController.h"

class AnimationComponent;

class AelorinSeekerSigilsState : public StateMachineScript
{
	DECLARE_SCRIPT(AelorinSeekerSigilsState)

public:
	explicit AelorinSeekerSigilsState(GameObject* owner);

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

private:
	AelorinBossController* m_controller = nullptr;
	AnimationComponent* m_animation = nullptr;

	AelorinAbility m_activeAbility = AelorinAbility::None;

	float m_testTimer = 0.0f;
	bool m_completed = false;
};
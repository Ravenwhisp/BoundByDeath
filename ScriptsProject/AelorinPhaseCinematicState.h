#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class AelorinBossController;
class AelorinCinematics;
class AnimationComponent;

class AelorinPhaseCinematicState : public StateMachineScript
{
	DECLARE_SCRIPT(AelorinPhaseCinematicState)

public:
	explicit AelorinPhaseCinematicState(GameObject* owner);

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

private:
	AelorinBossController* m_controller = nullptr;
	AelorinCinematics* m_cinematics = nullptr;
	AnimationComponent* m_animation = nullptr;

	bool m_transitionSent = false;
};
#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class AelorinBossController;
class AnimationComponent;

class AelorinPhaseTransition : public StateMachineScript
{
	DECLARE_SCRIPT(AelorinPhaseTransition)

public:
	explicit AelorinPhaseTransition(GameObject* owner);

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

private:
	AelorinBossController* m_controller = nullptr;
	AnimationComponent* m_animation = nullptr;

	bool m_phase2Started = false;
};
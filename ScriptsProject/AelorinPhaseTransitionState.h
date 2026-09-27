#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class AelorinBossController;
class AnimationComponent;
class AelorinVFX;

class AelorinPhaseTransitionState : public StateMachineScript
{
	DECLARE_SCRIPT(AelorinPhaseTransitionState)

public:
	explicit AelorinPhaseTransitionState(GameObject* owner);

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

private:
	AelorinBossController* m_controller = nullptr;
	AnimationComponent* m_animation = nullptr;
	AelorinVFX* m_vfx = nullptr;

	bool m_phase2Started = false;
};
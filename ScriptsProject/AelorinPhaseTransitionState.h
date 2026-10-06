#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class AelorinBossController;
class AnimationComponent;
class AelorinVFX;
class AelorinCinematics;

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
	AelorinCinematics* m_cinematics = nullptr;

	bool m_transformationStarted = false;
	bool m_phase2Started = false;
};
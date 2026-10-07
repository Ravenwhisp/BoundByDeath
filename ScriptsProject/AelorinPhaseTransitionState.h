#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class AelorinBossController;
class AnimationComponent;
class AelorinVFX;
class AelorinDamageable;

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
	AelorinDamageable* m_damageable = nullptr;

	bool m_dissolveStarted = false;
	bool m_phase2RevealStarted = false;

	float m_phase2RevealTimer = 0.0f;

	bool m_phase2Started = false;
};
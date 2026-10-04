#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class AelorinBossController;
class AnimationComponent;

class AelorinThresholdStaggerState : public StateMachineScript
{
	DECLARE_SCRIPT(AelorinThresholdStaggerState)

public:
	explicit AelorinThresholdStaggerState(GameObject* owner);

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

private:
	enum class StaggerPhase
	{
		Stagger,
		StaggerOut
	};

	void changePhase(StaggerPhase phase);
	void finishStagger();

private:
	AelorinBossController* m_controller = nullptr;
	AnimationComponent* m_animation = nullptr;

	StaggerPhase m_phase = StaggerPhase::Stagger;

	float m_staggerTimer = 0.0f;
	bool m_staggerCompleted = false;
};
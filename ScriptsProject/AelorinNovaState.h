#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

#include "AelorinBossController.h"

class AnimationComponent;

class AelorinNovaState : public StateMachineScript
{
	DECLARE_SCRIPT(AelorinNovaState)

public:
	explicit AelorinNovaState(GameObject* owner);

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

private:
	void applyFirstWave();
	void applySecondWave();
	void finishAbility();

private:
	AelorinBossController* m_controller = nullptr;
	AnimationComponent* m_animation = nullptr;

	AelorinAbility m_activeAbility = AelorinAbility::None;

	float m_stateTimer = 0.0f;

	bool m_firstWaveApplied = false;
	bool m_secondWaveApplied = false;
	bool m_completed = false;
};
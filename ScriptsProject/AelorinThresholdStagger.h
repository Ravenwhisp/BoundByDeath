#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class AelorinBossController;
class AnimationComponent;

class AelorinThresholdStagger : public StateMachineScript
{
	DECLARE_SCRIPT(AelorinThresholdStagger)

public:
	explicit AelorinThresholdStagger(GameObject* owner);

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

private:
	AelorinBossController* m_controller = nullptr;
	AnimationComponent* m_animation = nullptr;

	float m_staggerTimer = 0.0f;
	bool m_staggerCompleted = false;
};
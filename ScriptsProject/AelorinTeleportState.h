#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class AelorinBossController;
class AnimationComponent;

class AelorinTeleportState : public StateMachineScript
{
	DECLARE_SCRIPT(AelorinTeleportState)

public:
	explicit AelorinTeleportState(GameObject* owner);

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

private:
	AelorinBossController* m_controller = nullptr;
	AnimationComponent* m_animation = nullptr;
};
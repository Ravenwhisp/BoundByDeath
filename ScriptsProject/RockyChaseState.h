#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class RockyEnemyController;
class AnimationComponent;

class RockyChaseState : public StateMachineScript
{
	DECLARE_SCRIPT(RockyChaseState)

public:
	explicit RockyChaseState(GameObject* owner);

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

private:
	RockyEnemyController* m_rockyController = nullptr;
	AnimationComponent* m_animation = nullptr;
};
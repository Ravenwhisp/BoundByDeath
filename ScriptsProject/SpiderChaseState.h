#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class EnemyBaseController;
class AnimationComponent;

class SpiderChaseState : public StateMachineScript
{
	DECLARE_SCRIPT(SpiderChaseState)

public:
	explicit SpiderChaseState(GameObject* owner);

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

private:
	EnemyBaseController* m_spiderController = nullptr;
	AnimationComponent* m_animation = nullptr;
};
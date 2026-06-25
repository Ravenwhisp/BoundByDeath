#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class SkeletonEnemyController;
class SkeletonAttackConfig;
class AnimationComponent;

class SkeletonGuardState : public StateMachineScript
{
	DECLARE_SCRIPT(SkeletonGuardState)

public:
	explicit SkeletonGuardState(GameObject* owner);

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

private:
	SkeletonEnemyController* m_skeletonController = nullptr;
	SkeletonAttackConfig* m_attackConfig = nullptr;
	AnimationComponent* m_animation = nullptr;

	float m_stateTimer = 0.0f;
};
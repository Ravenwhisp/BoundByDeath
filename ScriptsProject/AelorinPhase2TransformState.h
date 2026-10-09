#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class AelorinBossController;
class AnimationComponent;
class DissolveComponent;

class AelorinPhase2TransformState : public StateMachineScript
{
	DECLARE_SCRIPT(AelorinPhase2TransformState)

public:
	explicit AelorinPhase2TransformState(GameObject* owner);

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

private:
	AelorinBossController* m_controller = nullptr;
	AnimationComponent* m_animation = nullptr;

	Transform* m_modelTransform = nullptr;
	DissolveComponent* m_dissolve = nullptr;

	Vector3 m_startScale = Vector3::One;
	Vector3 m_targetScale = Vector3::One;

	float m_stateTimer = 0.0f;
	bool m_completed = false;
};
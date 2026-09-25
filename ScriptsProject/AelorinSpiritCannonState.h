#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

#include "AelorinBossController.h"

class AnimationComponent;
class AelorinAttackExecutor;
class AelorinUI;

class AelorinSpiritCannonState : public StateMachineScript
{
	DECLARE_SCRIPT(AelorinSpiritCannonState)

public:
	explicit AelorinSpiritCannonState(GameObject* owner);

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

private:
	void selectTarget();
	bool isValidTarget(Transform* targetTransform) const;

	bool lockCurrentTargetPosition();

	void beginShot();
	void fireShot();
	void finishAbility();

private:
	AelorinBossController* m_controller = nullptr;
	AelorinAttackExecutor* m_attackExecutor = nullptr;
	AnimationComponent* m_animation = nullptr;
	AelorinUI* m_aelorinUI = nullptr;
	AelorinAbility m_activeAbility = AelorinAbility::None;

	Transform* m_aelorinTransform = nullptr;
	Transform* m_target = nullptr;

	Vector3 m_lockedTargetPosition = Vector3::Zero;
	Vector3 m_lockedAimDirection = Vector3::Zero;

	float m_shotTimer = 0.0f;
	float m_intervalTimer = 0.0f;
	float m_recoveryTimer = 0.0f;

	int m_shotCount = 0;

	bool m_shotActive = false;
	bool m_waitingForNextShot = false;
	bool m_recovering = false;
	bool m_completed = false;
	bool m_isFuryCast = false;
};
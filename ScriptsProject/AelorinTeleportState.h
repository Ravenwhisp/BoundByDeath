#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

#include "AelorinBossController.h"

class AnimationComponent;
class AelorinAttackExecutor;

class AelorinTeleportState : public StateMachineScript
{
	DECLARE_SCRIPT(AelorinTeleportState)

public:
	explicit AelorinTeleportState(GameObject* owner);

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

private:
	enum class TeleportPhase
	{
		TeleportIn,
		TeleportOut
	};

	void changePhase(TeleportPhase phase);
	void executeTeleport();
	void finishAbility();

private:
	AelorinBossController* m_controller = nullptr;
	AelorinAttackExecutor* m_attackExecutor = nullptr;
	AnimationComponent* m_animation = nullptr;

	Transform* m_aelorinTransform = nullptr;
	Transform* m_crowdingPlayer = nullptr;

	AelorinAbility m_activeAbility = AelorinAbility::None;

	TeleportPhase m_phase = TeleportPhase::TeleportIn;

	float m_phaseTimer = 0.0f;
	bool m_completed = false;
};
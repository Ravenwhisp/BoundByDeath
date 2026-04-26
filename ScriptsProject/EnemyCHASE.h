#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class Enemy_Controller;

class EnemyCHASE : public StateMachineScript
{
	DECLARE_SCRIPT(EnemyCHASE)

public:
	explicit EnemyCHASE(GameObject* owner);

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

	ScriptFieldList getExposedFields() const override;

private:
	Enemy_Controller* m_enemyController = nullptr;

public:
	bool m_debugEnabled = true;
	bool m_useCharge = false;
	float m_chargeTriggerRange = 4.0f;
};
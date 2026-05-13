#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class EnemyController;

class EnemyDAMAGETAKEN : public StateMachineScript
{
	DECLARE_SCRIPT(EnemyDAMAGETAKEN)

public:
	explicit EnemyDAMAGETAKEN(GameObject* owner);

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

	ScriptFieldList getExposedFields() const override;

private:
	EnemyController* m_enemyController = nullptr;
	float m_elapsedTime = 0.0f;

public:
	float m_duration = 0.35f;
	bool m_debugEnabled = true;
};

#pragma once

#include "ScriptAPI.h"
#include "StateMachineScript.h"

class Enemy_Controller;

class EnemyATTACK : public StateMachineScript
{
	DECLARE_SCRIPT(EnemyATTACK)

public:
	explicit EnemyATTACK(GameObject* owner);

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;

	ScriptFieldList getExposedFields() const override;

private:
	void performAttack();

private:
	Enemy_Controller* m_enemyController = nullptr;
	float m_attackTimer = 0.0f;

public:
	float m_attackDamage = 10.0f;
	float m_attackCooldown = 1.0f;
	bool m_debugEnabled = true;
};

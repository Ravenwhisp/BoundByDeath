#pragma once

#include "ScriptAPI.h"

class EnemyDetectionAggro : public Script
{
	DECLARE_SCRIPT(EnemyDetectionAggro)

public:
	explicit EnemyDetectionAggro(GameObject* owner);

	void Start() override;
	void Update() override;

	ScriptFieldList getExposedFields() const override;

public:
	// Exposed
	float m_detectionRadius = 10.0f;
	float m_loseAggroDelay = 2.0f;
	bool m_debugEnabled = true;

	Component* m_player1Transform = nullptr;
	Component* m_player2Transform = nullptr;

private:
	GameObject* m_target = nullptr; // current target
	bool m_isAggro = false; // is enemy in aggro state
	bool m_canSeeTarget = false; // is target detected
	float m_timeSinceLastSeen = 0.0f; // how much time has passed since detected target
	Vector3 m_lastKnownTargetPosition = Vector3(0.0f, 0.0f, 0.0f); // last known position of the target

private:
	bool isTargetValid() const;
	bool isTargetInDetectionRange() const;
	bool canDetectTarget() const;

	void enterAggro();
	void exitAggro();
	void updateAggroState();
};
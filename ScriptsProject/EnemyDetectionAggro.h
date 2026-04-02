#pragma once

#include "ScriptAPI.h"

class EnemyDetectionAggro : public Script
{
	DECLARE_SCRIPT(EnemyDetectionAggro)

public:
	explicit EnemyDetectionAggro(GameObject* owner);

	void Start() override;
	void Update() override;
	void drawGizmo() override;

	ScriptFieldList getExposedFields() const override;

public:
	// Exposed members
	float m_detectionRadius = 10.0f;
	float m_loseAggroDelay = 2.0f;
	float m_targetLockDuration = 1.5f; // how long the enemy stays lock on the current target
	bool m_debugEnabled = true;

	ScriptComponentRef<Transform> m_player1Transform;
	ScriptComponentRef<Transform> m_player2Transform;

private:
	Transform* m_currentTargetTransform = nullptr; // current target
	bool m_isAggro = false; // is enemy in aggro state
	bool m_canSeeTarget = false; // is target detected
	float m_timeSinceLastSeen = 0.0f; // how much time has passed since detected target
	Vector3 m_lastKnownTargetPosition = Vector3(0.0f, 0.0f, 0.0f); // last known position of the target

	float m_currentTargetLockTimer = 0.0f; // track current value of the lock timer

private:
	bool isTargetValid() const;
	bool isTargetInDetectionRange() const;
	bool canDetectTarget() const;

	void enterAggro(Transform* target);
	void exitAggro();
	void updateAggroState();

	bool isTargetLockActive() const;
	void startTargetLock();
	void updateTargetLockTimer();

	Transform* selectTargetInRange() const;

private:
	Transform* getOwnerTransform() const;
	Transform* getPlayer1Transform() const;
	Transform* getPlayer2Transform() const;

	Vector3 getOwnerPosition() const;
	Vector3 getPlayer1Position() const;
	Vector3 getPlayer2Position() const;

	float getDistanceToPlayer1() const;
	float getDistanceToPlayer2() const;

	bool isPlayer1InDetectionRange() const;
	bool isPlayer2InDetectionRange() const;
};
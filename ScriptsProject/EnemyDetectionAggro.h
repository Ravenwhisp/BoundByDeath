#pragma once

#include "ScriptAPI.h"

class EnemyDetectionAggro : public Script
{
	DECLARE_SCRIPT(EnemyDetectionAggro)

private:

	struct AggroEntry
	{
		Transform* targetTransform = nullptr;
		bool isInDetectionRange = false;
		float distanceToEnemy = 0.0f;
		float lastAttackTime = -9999.9f;
		float lastDamageTime = -9999.9f;
		float aggroScore = 0.0f;
	};

public:
	explicit EnemyDetectionAggro(GameObject* owner);

	void Start() override;
	void Update() override;
	void drawGizmo() override;

	ScriptFieldList getExposedFields() const override;

public:
	float m_detectionRadius = 10.0f;
	float m_loseAggroDelay = 2.0f;
	float m_targetLockDuration = 1.5f;
	bool m_debugEnabled = true;

	ScriptComponentRef<Transform> m_player1Transform;
	ScriptComponentRef<Transform> m_player2Transform;

public:
	void notifyPlayerAttackedEnemy(Transform* playerTransform);
	void notifyPlayerDealtDamage(Transform* playerTransform);

private:
	AggroEntry m_player1Aggro;
	AggroEntry m_player2Aggro;
	Transform* m_currentTargetTransform = nullptr;
	bool m_isAggro = false;
	bool m_canSeeTarget = false;
	float m_timeSinceLastSeen = 0.0f;
	Vector3 m_lastKnownTargetPosition = Vector3(0.0f, 0.0f, 0.0f);

	float m_currentTargetLockTimer = 0.0f;
	float m_currentTime = 0.0f;
	
	float m_distanceWeight = 1.0f;
	float m_recentAttackBonus = 50.0f;
	float m_recentDamageBonus = 60.0f;

	float m_recentAttackMemory = 3.0f;
	float m_recentDamageMemory = 3.0f;

	float m_targetSwitchThreshold = 10.0f;

private:

	void enterAggro(Transform* target);
	void exitAggro();
	void updateAggroState();
	void updateAggroEntries();
	void updateAggroScores();
	float calculateAggroScore(const AggroEntry& entry) const;

	bool isTargetLockActive() const;
	void startTargetLock();
	void updateTargetLockTimer();

	Transform* selectBestAggroTarget() const;

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

	AggroEntry* getAggroEntry(Transform* target);
	const AggroEntry* getAggroEntry(Transform* target) const;

	float getAggroScore(Transform* target) const;
};
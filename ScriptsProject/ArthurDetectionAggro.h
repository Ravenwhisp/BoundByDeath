#pragma once

#include "ScriptAPI.h"

enum class ArthurBossPhase
{
	Phase1,
	Phase2
};

class ArthurDetectionAggro : public Script
{
	DECLARE_SCRIPT(ArthurDetectionAggro)

private:
	struct AggroEntry
	{
		Transform* targetTransform = nullptr;
		bool isInDetectionRange = false;
		float distanceToEnemy = 0.0f;
		float lastAttackTime = -9999.9f;
	};

public:
	explicit ArthurDetectionAggro(GameObject* owner);

	void Start() override;
	void Update() override;
	void drawGizmo() override;

	ScriptFieldList getExposedFields() const override;

public:
	float m_detectionRadius = 10.0f;
	float m_targetLockDuration = 5.0f;
	bool m_debugEnabled = true;

	ScriptComponentRef<Transform> m_lyrielTransform;
	ScriptComponentRef<Transform> m_deathTransform;

public:
	void notifyPlayerAttackedEnemy(Transform* playerTransform);
	void applyTaunt(Transform* playerTransform, float durationSeconds);
	void clearTaunt(Transform* playerTransform);

	bool isAggro() const { return m_isAggro; }
	bool canSeeTarget() const { return m_canSeeTarget; }
	Transform* getCurrentTarget() const { return m_currentTargetTransform; }
	Vector3 getLastKnownTargetPosition() const { return m_lastKnownTargetPosition; }

	Transform* getLyrielTransform() const;
	Transform* getDeathTransform() const;

	void setPhase(ArthurBossPhase phase);
	ArthurBossPhase getPhase() const { return m_phase; }

	void startEncounter(); // called in controller
	void stopEncounter(); // called in controller

private:
	AggroEntry m_LyrielAggro;
	AggroEntry m_DeathAggro;

	Transform* m_currentTargetTransform = nullptr;
	bool m_isAggro = false;
	bool m_canSeeTarget = false;
	Vector3 m_lastKnownTargetPosition = Vector3(0.0f, 0.0f, 0.0f);
	Transform* m_tauntTargetTransform = nullptr;
	float m_tauntTimer = 0.0f;

	float m_currentTargetLockTimer = 0.0f;
	float m_currentTime = 0.0f;
	float m_recentAttackMemory = 3.0f;

	ArthurBossPhase m_phase = ArthurBossPhase::Phase1;
	bool m_encounterStarted = false;

private:
	void enterAggro(Transform* target);
	void updateAggroState();
	void updateAggroEntries();

	bool isTargetLockActive() const;
	void startTargetLock();
	void updateTargetLockTimer();
	void updateTauntTimer();
	bool isTaunted() const;

	Transform* selectClosestDetectedPlayer() const;
	Transform* selectReevaluatedTarget() const;

private:
	Transform* getOwnerTransform() const;

	Vector3 getOwnerPosition() const;
	Vector3 getLyrielPosition() const;
	Vector3 getDeathPosition() const;

	float getDistanceToLyriel() const;
	float getDistanceToDeath() const;

	bool isLyrielInDetectionRange() const;
	bool isDeathInDetectionRange() const;

	bool isLyrielAggroing() const;
	bool isDeathAggroing() const;
	bool isTransformAlive(Transform* target) const;

	AggroEntry* getAggroEntry(Transform* target);
	const AggroEntry* getAggroEntry(Transform* target) const;
};
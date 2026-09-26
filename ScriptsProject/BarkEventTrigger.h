#pragma once

#include "ScriptAPI.h"

class BarkEvent;
class Damageable;
class Bound;
class Transform;

enum class BarkTriggerActivationType
{
	BothPlayersEnter = 0,
	BothPlayersStay,
	DeathEnter,
	LyrielEnter,
	PlayerTakesDamage,
	FirstBoundSeparation,
	RepeatedBoundSeparation
};

class BarkEventTrigger : public Script
{
	DECLARE_SCRIPT(BarkEventTrigger)

public:
	explicit BarkEventTrigger(GameObject* owner);

	void Start() override;
	void Update() override;

	void OnTriggerEnter(GameObject* gameObject) override;
	void OnTriggerExit(GameObject* gameObject) override;

	FieldList getExposedFields() const override;

private:
	void setupZoneTrigger();
	void setupDamageTrigger();
	void setupBoundTrigger();

	void updateZoneEnter();
	void updateZoneStay();
	void updatePlayerEnter();
	void updateDamage();
	void updateBound();

	void findPlayers();

	void setPlayerInside(
		GameObject* gameObject,
		bool inside
	);

	bool areBothPlayersInside() const;
	bool isSelectedPlayerInside() const;

	bool isBoundSeparated() const;

	bool triggerBark();

	void resetCurrentZoneActivation();

private:
	int m_activationType =
		static_cast<int>(
			BarkTriggerActivationType::BothPlayersEnter
			);

	bool m_triggerOnlyOnce = true;

	float m_delay = 0.0f;
	float m_requiredStayTime = 5.0f;

	// Player whose HP will be observed.
	// Only used by PlayerTakesDamage.
	ComponentRef<Transform> m_damageTarget;

	// GameObject containing the Bound script.
	// Only used by Bound separation triggers.
	ComponentRef<Transform> m_boundObject;

	BarkEvent* m_barkEvent = nullptr;

	// Zone conditions.
	GameObject* m_death = nullptr;
	GameObject* m_lyriel = nullptr;

	bool m_deathInside = false;
	bool m_lyrielInside = false;

	bool m_hasTriggered = false;

	bool m_hasTriggeredCurrentOccupancy = false;
	bool m_isWaitingForDelay = false;

	float m_timer = 0.0f;

	// Damage condition.
	Damageable* m_observedDamageable = nullptr;
	float m_previousHp = 0.0f;

	// Bound condition.
	Bound* m_bound = nullptr;

	bool m_boundStateInitialized = false;
	bool m_wasSeparated = false;
	bool m_hasSeenFirstSeparation = false;
};
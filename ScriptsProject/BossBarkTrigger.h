#pragma once

#include "ScriptAPI.h"

#include <string>

class BarkEvent;
class Damageable;
class Transform;
class AelorinBossController;
class ArthurBossController;
class AnimationComponent;

enum class BossBarkConditionType
{
	EncounterTime = 0,
	HealthPercentBelow,
	AnimationStateEnter,
	PhaseEnter
};

enum class BossBarkPhaseFilter
{
	Any = 0,
	Phase1,
	Phase2
};

// Dispara un bark por el estado del jefe, no por una zona. Vive en el mismo GameObject que su
// BarkEvent y lee al jefe desde fuera, sin tocar su controller.
class BossBarkTrigger : public Script
{
	DECLARE_SCRIPT(BossBarkTrigger)

public:
	explicit BossBarkTrigger(GameObject* owner);

	void Start() override;
	void Update() override;

	FieldList getExposedFields() const override;

public:
	int m_conditionType = static_cast<int>(BossBarkConditionType::AnimationStateEnter);

	ComponentRef<Transform> m_bossObject;

	float m_encounterTime = 30.0f;
	float m_healthPercent = 0.5f;

	std::string m_animationStateName;
	int m_requiredOccurrence = 1;

	int m_phaseFilter = static_cast<int>(BossBarkPhaseFilter::Any);

	float m_barkDelay = 0.0f;
	bool m_barkOnlyOnce = true;

private:
	bool isEncounterActive() const;
	bool passesPhaseFilter() const;
	int currentPhase() const;

	AnimationComponent* resolveAnimation() const;
	const char* activeStateName() const;

	bool evaluateCondition();
	void requestBark();
	bool playBark();   // true solo si ha sonado de verdad

private:
	BarkEvent* m_barkEvent = nullptr;

	GameObject* m_boss = nullptr;
	Damageable* m_damageable = nullptr;
	AelorinBossController* m_aelorin = nullptr;
	ArthurBossController* m_arthur = nullptr;

	bool m_encounterWasActive = false;
	float m_encounterTimer = 0.0f;

	std::string m_previousStateName;
	int m_stateEnterCount = 0;

	int m_previousPhase = -1;

	bool m_hasPlayed = false;
	bool m_waitingForDelay = false;
	float m_delayTimer = 0.0f;
};

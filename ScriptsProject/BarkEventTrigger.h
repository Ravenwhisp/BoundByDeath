#pragma once

#include "ScriptAPI.h"

class BarkEvent;

enum class BarkTriggerActivationType
{
	BothPlayersEnter = 0,
	BothPlayersStay
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
	void findPlayers();
	void setPlayerInside(GameObject* gameObject, bool inside);

	bool areBothPlayersInside() const;

	void triggerBark();
	void resetCurrentActivation();

private:
	int m_activationType =
		static_cast<int>(BarkTriggerActivationType::BothPlayersEnter);

	bool m_triggerOnlyOnce = true;

	float m_delay = 0.0f;
	float m_requiredStayTime = 5.0f;

	GameObject* m_death = nullptr;
	GameObject* m_lyriel = nullptr;

	BarkEvent* m_barkEvent = nullptr;

	bool m_deathInside = false;
	bool m_lyrielInside = false;

	bool m_hasTriggered = false;
	bool m_hasTriggeredCurrentOccupancy = false;
	bool m_isWaitingForDelay = false;

	float m_timer = 0.0f;
};
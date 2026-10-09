#pragma once

#include "ScriptAPI.h"

class TriggerFire : public Script
{
	DECLARE_SCRIPT(TriggerFire);


public:
	explicit TriggerFire(GameObject* owner);

	void Start() override;
	void Update() override;
	void OnTriggerEnter(GameObject* gameObject) override;

	FieldList getExposedFields() const override;

private:

	void triggerFire();

	ComponentRef<Transform> m_fireEffectT;
	ComponentRef<Transform> m_lightT;

	bool m_fireTriggered = false;

	bool  m_soundPending = false;
	float m_soundDelay = 0.0f;
	int   m_soundRetries = 0;
};


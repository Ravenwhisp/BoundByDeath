#pragma once

#include "ScriptAPI.h"

#include <vector>

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

	// Emisores desde los que suena el encendido. Si esta vacio, suena desde el objeto de
	// fuego, que es como estaban montadas las antorchas del nivel 2. Hace falta cuando el
	// objeto de fuego no esta donde esta la llama: los braseros del jefe comparten un
	// disparador por pareja y su objeto de fuego vive en el origen del mundo.
	std::vector<ComponentRef<Transform>> m_soundEmitters;
	ComponentRef<Transform> m_lightT;

	bool m_fireTriggered = false;

	bool  m_soundPending = false;
	float m_soundDelay = 0.0f;
	int   m_soundRetries = 0;
};


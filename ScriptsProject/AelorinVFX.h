#pragma once

#include "ScriptAPI.h"
#include "ParticleLifecycle.h"

class AelorinVFX : public Script
{
	DECLARE_SCRIPT(AelorinVFX)

public:
	explicit AelorinVFX(GameObject* owner);

	void OnGameStop() override;

	FieldList getExposedFields() const override;

	void startPhase2Aura();
	void stopPhase2Aura();

public:
	PrefabRef m_phase2AuraPrefab;

private:
	GameObject* m_phase2AuraEffect = nullptr;
	bool m_phase2AuraActive = false;
};
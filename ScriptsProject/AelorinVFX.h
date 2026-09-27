#pragma once

#include "ScriptAPI.h"
#include "ParticleLifecycle.h"

class AelorinVFX : public Script
{
	DECLARE_SCRIPT(AelorinVFX)

public:
	explicit AelorinVFX(GameObject* owner);

	void Update() override;
	void OnGameStop() override;

	FieldList getExposedFields() const override;

	// Phase 2 Aura
	void startPhase2Aura();
	void stopPhase2Aura();

	// Phase 2 Transition
	void playPhase2Transition();

public:
	// Phase 2 Aura
	PrefabRef m_phase2AuraPrefab;

	// Phase 2 Transition
	PrefabRef m_phase2TransitionPrefab;

private:
	// Phase 2 Aura
	GameObject* m_phase2AuraEffect = nullptr;
	bool m_phase2AuraActive = false;

	// Timed Tracker
	ParticleLifecycle::TimedParticleTracker m_oneShotVFX;
};
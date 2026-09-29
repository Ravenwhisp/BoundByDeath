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

	// Phase 1 Teleport
	void startPhase1Teleport();
	void stopPhase1Teleport();

	// Phase 2 Teleport
	void startPhase2Teleport();
	void stopPhase2Teleport();

	// Summon Enemy
	void playSummonEnemyEffect(const Vector3& position);

	// Phase 1 Spellcasting
	void startPhase1Spell();
	void stopPhase1Spell();

	// Phase 2 Spellcasting
	//void startPhase2Spell();
	//void stopPhase2Spell();

public:
	// Phase 2 Aura
	PrefabRef m_phase2AuraPrefab;

	// Phase 2 Transition
	PrefabRef m_phase2TransitionPrefab;

	// Phase 1 Teleport
	PrefabRef m_phase1TeleportPrefab;

	// Phase 2 Teleport
	PrefabRef m_phase2TeleportPrefab;

	// Summon Enemy
	PrefabRef m_summonEnemyPrefab;

	// Phase 1 Spellcasting
	PrefabRef m_phase1SpellPrefab;

	// Phase 2 Spellcasting
	PrefabRef m_phase2SpellPrefab;

	// Component References
	ComponentRef<Transform> m_phase1RightHand;
	ComponentRef<Transform> m_phase1LeftHand;

private:
	// Phase 2 Aura
	GameObject* m_phase2AuraEffect = nullptr;
	bool m_phase2AuraActive = false;

	// Phase 1 Teleport
	GameObject* m_phase1TeleportEffect = nullptr;
	bool m_phase1TeleportActive = false;

	// Phase 2 Teleport
	GameObject* m_phase2TeleportEffect = nullptr;
	bool m_phase2TeleportActive = false;

	// Phase 1 Spellcasting
	GameObject* m_phase1RightSpellEffect = nullptr;
	GameObject* m_phase1LeftSpellEffect = nullptr;
	bool m_phase1SpellActive = false;

	// Phase 2 Spellcasting
	GameObject* m_phase2RightSpellEffect = nullptr;
	GameObject* m_phase2LeftSpellEffect = nullptr;
	bool m_phase2SpellActive = false;

	// Timed Tracker
	ParticleLifecycle::TimedParticleTracker m_oneShotVFX;
};
#pragma once

#include "ScriptAPI.h"

class AelorinArenaManager : public Script
{
	DECLARE_SCRIPT(AelorinArenaManager)

public:
	explicit AelorinArenaManager(GameObject* owner);

	FieldList getExposedFields() const override;

	void Start() override;
	
	void switchToPhase2();

private:
	void moveToPhase2(Transform* target);

private:
	// Arena origins
	ComponentRef<Transform> m_phase1Origin;
	ComponentRef<Transform> m_phase2Origin;

	// Encounter Roots
	ComponentRef<Transform> m_aelorinRoot;
	ComponentRef<Transform> m_lyrielRoot;
	ComponentRef<Transform> m_deathRoot;

	// Camera
	ComponentRef<Transform> m_cinematics;
	ComponentRef<Transform> m_arenaCamera;
	ComponentRef<Transform> m_combatCamera;

	Transform* m_phase1OriginTransform = nullptr;
	Transform* m_phase2OriginTransform = nullptr;

	Transform* m_aelorinRootTransform = nullptr;
	Transform* m_lyrielRootTransform = nullptr;
	Transform* m_deathRootTransform = nullptr;

	Transform* m_cinematicsTransform = nullptr;
	Transform* m_arenaCameraTransform = nullptr;
	Transform* m_combatCameraTransform = nullptr;

	bool m_phase2Active = false;
};
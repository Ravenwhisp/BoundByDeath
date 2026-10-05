#pragma once
#include <Script.h>
#include "ScriptAPI.h"
#include "ParticleLifecycle.h"

#include <string>
#include <vector>

class LyrielParticles : public Script
{
	DECLARE_SCRIPT(LyrielParticles)

public:
	explicit LyrielParticles(GameObject* owner);

	void Start() override;
	void Update() override;
	void OnGameStop() override;

	FieldList getExposedFields() const override;

	void SetDashActive();
	void SetDashInactive();

	void SetChargeActive();
	void SetChargeInactive();

	void playHitFlash(const Vector3& position, GameObject* target);

	ComponentRef<Transform> m_dashTrail;
	PrefabRef m_chargeGlowPrefab;
	PrefabRef m_dashParticlePrefab;
	PrefabRef m_hitFlashPrefab;

	std::string m_chargeGlowPath = "Assets/Prefabs/Particles/Lyriel/LyrielChargeGlow.prefab";
	std::string m_dashParticlePath = "Assets/Prefabs/Particles/Lyriel/LyrielDashParticles.prefab";
	std::string m_hitFlashPath = "Assets/Prefabs/Particles/Lyriel/LyrielHitFlash.prefab";
	std::string m_bowAnchorName = "ArrowSpawn";
	int m_hitFlashPoolSize = 8;

private:
	struct HitFlashSlot
	{
		GameObject* instance = nullptr;
		GameObject* target = nullptr;
		Vector3 targetOffset = Vector3::Zero;
		float remainingSeconds = 0.0f;
	};

	Transform* getTransform(ComponentRef<Transform> controller);
	Transform* findBowTransform() const;
	void syncActiveParticles();
	void prewarmHitFlashes();
	void updateHitFlashes(float deltaTime);

	Transform* m_dashTrailController = nullptr;
	GameObject* m_chargeGlowInstance = nullptr;
	GameObject* m_dashParticleInstance = nullptr;
	bool m_chargeGlowActive = false;
	bool m_dashParticleActive = false;
	std::vector<HitFlashSlot> m_hitFlashPool;
};

#pragma once

#include "ProjectileBase.h"

class AelorinAttackExecutor;
class CameraShake;
class AelorinVFX;

class SeekerSigilProjectile : public ProjectileBase
{
	DECLARE_SCRIPT(SeekerSigilProjectile)

public:
	explicit SeekerSigilProjectile(GameObject* owner);

	void Update() override;

	void launch(
		const Vector3& startPosition,
		const Vector3& targetPosition,
		float fallSpeed,
		float lifetime,
		float impactRadius,
		float damage,
		AelorinAttackExecutor* attackExecutor,
		CameraShake* cameraShake,
		AelorinVFX* vfx,
		bool heavyImpact
	);

	void resetProjectile() override;

private:
	void applyImpactDamage();
	void playImpactFeedback();

private:
	AelorinAttackExecutor* m_attackExecutor = nullptr;
	CameraShake* m_cameraShake = nullptr;
	AelorinVFX* m_vfx = nullptr;
	Vector3 m_targetPosition = Vector3::Zero;

	float m_fallSpeed = 0.0f;
	float m_lifetime = 0.0f;
	float m_aliveTimer = 0.0f;
	float m_damage = 0.0f;
	float m_impactRadius = 0.5f;

	bool m_isLaunched = false;
	bool m_heavyImpact = false;
};
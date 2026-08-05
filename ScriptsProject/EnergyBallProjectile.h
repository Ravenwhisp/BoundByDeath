#pragma once

#include "ProjectileBase.h"

class EnergyBallProjectile : public ProjectileBase
{
	DECLARE_SCRIPT(EnergyBallProjectile)

public:
	explicit EnergyBallProjectile(GameObject* owner);

	void Update() override;

	FieldList getExposedFields() const override;

	void launch(const Vector3& startPosition, const Vector3& direction, float speed, float lifetime, GameObject* target, float damage);
	void resetProjectile() override;

private:
	void applyImpactDamage();

	void spawnEnergyBallParticles();
	void updateEnergyBallParticles();
	void removeEnergyBallParticles();

public: 
	PrefabRef m_energyBallSparks1Prefab;
	PrefabRef m_energyBallSparks2Prefab;

private:
	GameObject* m_energyBallSparks1 = nullptr;
	GameObject* m_energyBallSparks2 = nullptr;

	Transform* m_energyBallSparks1Transform = nullptr;
	Transform* m_energyBallSparks2Transform = nullptr;

	Vector3 m_direction = Vector3::Zero;

	float m_speed = 0.0f;
	float m_lifetime = 0.0f;
	float m_aliveTimer = 0.0f;
	float m_damage = 0.0f;
	float m_hitRadius = 0.5f;

	GameObject* m_target = nullptr;

	bool m_isLaunched = false;
};
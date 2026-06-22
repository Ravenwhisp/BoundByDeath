#pragma once

#include "ScriptAPI.h"

class EnergyBallProjectile : public Script
{
	DECLARE_SCRIPT(EnergyBallProjectile)

public:
	explicit EnergyBallProjectile(GameObject* owner);

	void Update() override;

	void launch(const Vector3& direction, float speed, float damage);

private:
	Vector3 m_direction = Vector3::Zero;
	float m_speed = 0.0f;
	float m_damage = 0.0f;
	bool m_isLaunched = false;
};
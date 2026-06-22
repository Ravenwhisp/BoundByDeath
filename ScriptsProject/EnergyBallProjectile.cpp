#include "pch.h"
#include "EnergyBallProjectile.h"

EnergyBallProjectile::EnergyBallProjectile(GameObject* owner)
	: Script(owner)
{
}

void EnergyBallProjectile::launch(const Vector3& direction, float speed, float damage)
{
	m_direction = direction;

	if (m_direction.LengthSquared() > 0.00001f)
	{
		m_direction.Normalize();
	}

	m_speed = speed;
	m_damage = damage;
	m_isLaunched = true;
}

void EnergyBallProjectile::Update()
{
	if (!m_isLaunched)
	{
		return;
	}

	Transform* transform = GameObjectAPI::getTransform(getOwner());
	if (!transform)
	{
		return;
	}

	Vector3 currentPosition = TransformAPI::getPosition(transform);
	Vector3 nextPosition = currentPosition + m_direction * m_speed * Time::getDeltaTime();

	TransformAPI::setPosition(transform, nextPosition);
}

IMPLEMENT_SCRIPT(EnergyBallProjectile)
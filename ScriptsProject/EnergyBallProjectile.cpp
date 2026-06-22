#include "pch.h"
#include "EnergyBallProjectile.h"

#include "Damageable.h"
#include "PlayerState.h"

EnergyBallProjectile::EnergyBallProjectile(GameObject* owner)
	: Script(owner)
{
}

void EnergyBallProjectile::launch(const Vector3& startPosition, const Vector3& direction, float speed, float lifetime, GameObject* target, float damage)
{
	m_direction = direction;

	if (m_direction.LengthSquared() > 0.00001f)
	{
		m_direction.Normalize();
	}

	m_speed = speed;
	m_lifetime = lifetime;
	m_aliveTimer = 0.0f;
	m_target = target;
	m_damage = damage;
	m_isLaunched = true;

	Transform* transform = GameObjectAPI::getTransform(getOwner());
	if (transform)
	{
		TransformAPI::setGlobalPosition(transform, startPosition);
		TransformAPI::lookAt(transform, startPosition + m_direction);
	}
}

void EnergyBallProjectile::Update()
{
	if (!m_isLaunched)
	{
		return;
	}

	m_aliveTimer += Time::getDeltaTime();

	if (m_aliveTimer >= m_lifetime)
	{
		GameObjectAPI::removeGameObject(getOwner());
		return;
	}

	if (!m_target)
	{
		GameObjectAPI::removeGameObject(getOwner());
		return;
	}

	Transform* projectileTransform = GameObjectAPI::getTransform(getOwner());
	Transform* targetTransform = GameObjectAPI::getTransform(m_target);

	if (!projectileTransform || !targetTransform)
	{
		GameObjectAPI::removeGameObject(getOwner());
		return;
	}

	Vector3 projectilePosition = TransformAPI::getGlobalPosition(projectileTransform);
	Vector3 targetPosition = TransformAPI::getGlobalPosition(targetTransform);

	Vector3 toTarget = targetPosition - projectilePosition;
	toTarget.y = 0.0f;

	const float distanceSquared = toTarget.LengthSquared();
	const float hitRadiusSquared = m_hitRadius * m_hitRadius;

	if (distanceSquared <= hitRadiusSquared)
	{
		applyImpactDamage();
		GameObjectAPI::removeGameObject(getOwner());
		return;
	}

	if (distanceSquared <= 0.00001f)
	{
		return;
	}

	toTarget.Normalize();
	m_direction = toTarget;

	TransformAPI::translateGlobal(projectileTransform, m_direction * m_speed * Time::getDeltaTime());
	TransformAPI::lookAt(projectileTransform, projectilePosition + m_direction);
}

void EnergyBallProjectile::applyImpactDamage()
{
	if (!m_target)
	{
		return;
	}

	PlayerState* playerState = GameObjectAPI::findScript<PlayerState>(m_target);
	if (playerState && playerState->isDowned())
	{
		return;
	}

	Damageable* damageable = GameObjectAPI::findScript<Damageable>(m_target);
	if (!damageable)
	{
		return;
	}

	damageable->takeDamage(m_damage);

	Debug::log("[EnergyBallProjectile] Damaged target for %.2f.", m_damage);
}

IMPLEMENT_SCRIPT(EnergyBallProjectile)
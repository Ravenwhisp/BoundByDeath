#include "pch.h"
#include "EnergyBallProjectile.h"

#include "Damageable.h"
#include "PlayerState.h"

IMPLEMENT_SCRIPT_FIELDS(EnergyBallProjectile,
	SERIALIZED_ASSET_REF(m_energyBallSparks1Prefab, "Energy Ball Sparks 1 Prefab", AssetType::PREFAB),
	SERIALIZED_ASSET_REF(m_energyBallSparks2Prefab, "Energy Ball Sparks 2 Prefab", AssetType::PREFAB)
)

EnergyBallProjectile::EnergyBallProjectile(GameObject* owner)
	: ProjectileBase(owner)
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
	m_inUse = true;

	GameObjectAPI::setActive(getOwner(), true);

	Transform* transform = GameObjectAPI::getTransform(getOwner());

	if (transform)
	{
		TransformAPI::setGlobalPosition(transform, startPosition);
		TransformAPI::lookAt(transform, startPosition + m_direction);
	}

	spawnEnergyBallParticles();
}

void EnergyBallProjectile::resetProjectile()
{
	removeEnergyBallParticles();

	m_direction = Vector3::Zero;

	m_speed = 0.0f;
	m_lifetime = 0.0f;
	m_aliveTimer = 0.0f;
	m_damage = 0.0f;

	m_target = nullptr;
	m_isLaunched = false;

	ProjectileBase::resetProjectile();
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
		returnToPool();
		return;
	}

	if (!m_target)
	{
		returnToPool();
		return;
	}

	Transform* projectileTransform = GameObjectAPI::getTransform(getOwner());
	Transform* targetTransform = GameObjectAPI::getTransform(m_target);

	if (!projectileTransform || !targetTransform)
	{
		returnToPool();
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
		returnToPool();
		return;
	}

	if (distanceSquared <= 0.00001f)
	{
		return;
	}

	toTarget.Normalize();
	m_direction = toTarget;

	TransformAPI::translateGlobal(projectileTransform, m_direction * m_speed * Time::getDeltaTime());

	const Vector3 updatedPosition = TransformAPI::getGlobalPosition(projectileTransform);
	TransformAPI::lookAt(projectileTransform, updatedPosition + m_direction);

	updateEnergyBallParticles();
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

void EnergyBallProjectile::spawnEnergyBallParticles()
{
	removeEnergyBallParticles();

	Transform* projectileTransform = GameObjectAPI::getTransform(getOwner());

	if (!projectileTransform)
	{
		return;
	}

	const Vector3 position = TransformAPI::getGlobalPosition(projectileTransform);
	const Vector3 rotation = TransformAPI::getGlobalEulerDegrees(projectileTransform);

	if (m_energyBallSparks1Prefab.m_id.isValid())
	{
		m_energyBallSparks1 = GameObjectAPI::instantiatePrefab(m_energyBallSparks1Prefab.m_id, position, rotation);

		if (m_energyBallSparks1)
		{
			m_energyBallSparks1Transform = GameObjectAPI::getTransform(m_energyBallSparks1);
		}
	}

	if (m_energyBallSparks2Prefab.m_id.isValid())
	{
		m_energyBallSparks2 = GameObjectAPI::instantiatePrefab(m_energyBallSparks2Prefab.m_id, position, rotation);

		if (m_energyBallSparks2)
		{
			m_energyBallSparks2Transform = GameObjectAPI::getTransform(m_energyBallSparks2);
		}
	}
}

void EnergyBallProjectile::updateEnergyBallParticles()
{
	Transform* projectileTransform = GameObjectAPI::getTransform(getOwner());

	if (!projectileTransform)
	{
		return;
	}

	const Vector3 position = TransformAPI::getGlobalPosition(projectileTransform);
	const Vector3 rotation = TransformAPI::getGlobalEulerDegrees(projectileTransform);

	if (m_energyBallSparks1Transform)
	{
		TransformAPI::setGlobalPosition(m_energyBallSparks1Transform, position);
		TransformAPI::setGlobalRotationEuler(m_energyBallSparks1Transform, rotation);
	}

	if (m_energyBallSparks2Transform)
	{
		TransformAPI::setGlobalPosition(m_energyBallSparks2Transform, position);
		TransformAPI::setGlobalRotationEuler(m_energyBallSparks2Transform, rotation);
	}
}

void EnergyBallProjectile::removeEnergyBallParticles()
{
	if (m_energyBallSparks1)
	{
		GameObjectAPI::removeGameObject(m_energyBallSparks1);
	}

	if (m_energyBallSparks2)
	{
		GameObjectAPI::removeGameObject(m_energyBallSparks2);
	}

	m_energyBallSparks1 = nullptr;
	m_energyBallSparks2 = nullptr;

	m_energyBallSparks1Transform = nullptr;
	m_energyBallSparks2Transform = nullptr;
}

IMPLEMENT_SCRIPT(EnergyBallProjectile)
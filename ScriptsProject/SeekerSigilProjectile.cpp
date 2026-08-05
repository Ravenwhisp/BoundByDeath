#include "pch.h"
#include "SeekerSigilProjectile.h"

#include "Damageable.h"
#include "PlayerState.h"

SeekerSigilProjectile::SeekerSigilProjectile(GameObject* owner)
	: ProjectileBase(owner)
{
}

void SeekerSigilProjectile::launch(const Vector3& startPosition, const Vector3& targetPosition,	float fallSpeed, float lifetime, float impactRadius, float damage)
{
	m_targetPosition = targetPosition;

	m_fallSpeed = fallSpeed;
	m_lifetime = lifetime;
	m_aliveTimer = 0.0f;

	m_impactRadius = impactRadius;
	m_damage = damage;

	m_isLaunched = true;
	m_inUse = true;

	GameObjectAPI::setActive(getOwner(), true);

	Transform* transform = GameObjectAPI::getTransform(getOwner());

	if (!transform)
	{
		return;
	}

	TransformAPI::setGlobalPosition(transform, startPosition);

	TransformAPI::lookAt(transform,	m_targetPosition);
}

void SeekerSigilProjectile::Update()
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

	Transform* projectileTransform = GameObjectAPI::getTransform(getOwner());

	if (!projectileTransform)
	{
		returnToPool();
		return;
	}

	const Vector3 currentPosition = TransformAPI::getGlobalPosition(projectileTransform);

	Vector3 toTarget = m_targetPosition - currentPosition;

	const float distanceSquared = toTarget.LengthSquared();

	const float movementThisFrame =	m_fallSpeed * Time::getDeltaTime();

	if (distanceSquared <= movementThisFrame * movementThisFrame)
	{
		TransformAPI::setGlobalPosition(projectileTransform, m_targetPosition);

		applyImpactDamage();
		returnToPool();
		return;
	}

	if (distanceSquared <= 0.00001f)
	{
		applyImpactDamage();
		returnToPool();
		return;
	}

	toTarget.Normalize();

	TransformAPI::translateGlobal(projectileTransform, toTarget * movementThisFrame);

	TransformAPI::lookAt(projectileTransform, currentPosition + toTarget);
}

void SeekerSigilProjectile::applyImpactDamage()
{
	const std::vector<GameObject*> players = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER);

	for (GameObject* player : players)
	{
		if (!player)
		{
			continue;
		}

		PlayerState* playerState = GameObjectAPI::findScript<PlayerState>(player);

		if (playerState && playerState->isDowned())
		{
			continue;
		}

		Transform* playerTransform = GameObjectAPI::getTransform(player);

		if (!playerTransform)
		{
			continue;
		}

		Vector3 playerPosition = TransformAPI::getGlobalPosition(playerTransform);

		Vector3 difference = playerPosition - m_targetPosition;

		difference.y = 0.0f;

		if (difference.LengthSquared() > m_impactRadius * m_impactRadius)
		{
			continue;
		}

		Damageable* damageable = GameObjectAPI::findScript<Damageable>(player);

		if (!damageable)
		{
			continue;
		}

		damageable->takeDamage(m_damage);
	}

	Debug::log(
		"[SeekerSigilProjectile] Impact at position "
		"(%.2f, %.2f, %.2f).",
		m_targetPosition.x,
		m_targetPosition.y,
		m_targetPosition.z
	);
}

void SeekerSigilProjectile::resetProjectile()
{
	m_targetPosition = Vector3::Zero;

	m_fallSpeed = 0.0f;
	m_lifetime = 0.0f;
	m_aliveTimer = 0.0f;

	m_impactRadius = 0.0f;
	m_damage = 0.0f;

	m_isLaunched = false;

	ProjectileBase::resetProjectile();
}

IMPLEMENT_SCRIPT(SeekerSigilProjectile)
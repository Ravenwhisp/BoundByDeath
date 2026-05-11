#include "pch.h"
#include "HealthPickup.h"

#include "Damageable.h"

IMPLEMENT_SCRIPT_FIELDS(HealthPickup, 
	SERIALIZED_FLOAT(m_healAmount, "Heal Amount", 0.0f, 100.0f, 1.0f),
	SERIALIZED_FLOAT(m_detectionRadius, "Detection Radius", 0.0f, 100.0f, 0.1f),
	SERIALIZED_FLOAT(m_maxFlySpeed, "Max Fly Speed", 0.0f, 50.0f, 0.1f),
	SERIALIZED_FLOAT(m_slowDetectionRadius, "Slow Detection Radius", 0.0f, 100.0f, 0.1f),
	SERIALIZED_FLOAT(m_slowSpeed, "Slow Speed", 0.0f, 5.0f, 0.1f)
    )

HealthPickup::HealthPickup(GameObject* owner)
    : Pickup(owner)
{
}

void HealthPickup::Start()
{
	playerObjects = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER);
}

void HealthPickup::Update()
{
	if (m_collected)
	{
		return;
	}

	const float dt = Time::getDeltaTime();

	const Vector3 position = TransformAPI::getPosition(GameObjectAPI::getTransform(getOwner()));
	GameObject* closestPlayer = nullptr;
	const float distance = getDistanceToClosestPlayer(position, closestPlayer);

	if(distance > m_slowDetectionRadius * m_slowDetectionRadius)
	{
		return;
	}

	if (closestPlayer != nullptr)
	{
		moveTowardsClosestPlayer(closestPlayer, dt);
	}
}

void HealthPickup::OnTriggerEnter(GameObject* player)
{
	if (!player || GameObjectAPI::getTag(player) != Tag::PLAYER)
	{
		return;
	}

	for(int i = 0; i < playerObjects.size(); ++i)
	{
		Damageable* damageable = GameObjectAPI::findScript<Damageable>(playerObjects[i]);
		if (damageable != nullptr)
		{
			damageable->heal(m_healAmount);
		}
		GameObjectAPI::removeGameObject(getOwner());
	}

	Pickup::OnTriggerEnter(player);
}

void HealthPickup::drawGizmo()
{
	DebugDrawAPI::drawCircle(TransformAPI::getPosition(GameObjectAPI::getTransform(getOwner())), Vector3::UnitY, { 1.0f, 0.0f, 1.0f }, m_detectionRadius);
	DebugDrawAPI::drawCircle(TransformAPI::getPosition(GameObjectAPI::getTransform(getOwner())), Vector3::UnitY, { 1.0f, 1.0f, 0.0f }, m_slowDetectionRadius);
}

void HealthPickup::moveTowardsClosestPlayer(const GameObject* closestPlayer, const float dt)
{
	const Vector3 pickupPosition = TransformAPI::getPosition(GameObjectAPI::getTransform(getOwner()));
	const Vector3 playerPosition = TransformAPI::getPosition(GameObjectAPI::getTransform(closestPlayer));

	Vector3 directionToPlayer = playerPosition - pickupPosition;
	const float distanceToPlayer = directionToPlayer.Length();

	if (!m_isLockedOnPlayer && distanceToPlayer <= m_detectionRadius)
	{
		m_isLockedOnPlayer = true;
		m_startHeight = pickupPosition.y;
	}

	if (m_isLockedOnPlayer || distanceToPlayer <= m_slowDetectionRadius)
	{
		float speed = m_slowSpeed;
		float nextY = pickupPosition.y;

		if (m_isLockedOnPlayer)
		{
			m_lockOnTimer += dt * 2.0f;
			float progress = (std::min)(1.0f, m_lockOnTimer);

			speed = m_slowSpeed + (progress * progress * (m_maxFlySpeed - m_slowSpeed));

			float targetHeightAtChest = playerPosition.y + 0.5f;
			float expFactor = progress * progress;
			float desiredY = m_startHeight + (targetHeightAtChest - m_startHeight) * expFactor;

			nextY = (pickupPosition.y * 0.8f) + (desiredY * 0.2f);
		}

		Vector3 dirHorizontal = directionToPlayer;
		dirHorizontal.y = 0;
		dirHorizontal.Normalize();

		Vector3 newPosition = pickupPosition + (dirHorizontal * speed * dt);
		newPosition.y = nextY;

		Transform* transform = GameObjectAPI::getTransform(getOwner());
		TransformAPI::setPosition(transform, newPosition);
	}
}

float HealthPickup::getDistanceToClosestPlayer(const Vector3& position, GameObject*& closestPlayer) const
{
	float result = 0.f;

	for (int i = 0; i < playerObjects.size(); ++i)
	{
		if (playerObjects[i] == nullptr)
		{
			continue;
		}
		Vector3 playerPosition = TransformAPI::getPosition(GameObjectAPI::getTransform(playerObjects[i]));
		if (i == 0)
		{
			result = Vector3::DistanceSquared(position, playerPosition);
			closestPlayer = playerObjects[i];
		}
		else
		{
			float distanceSq = Vector3::DistanceSquared(position, playerPosition);
			if (distanceSq < result)
			{
				result = distanceSq;
				closestPlayer = playerObjects[i];
			}
		}
	}

	return result;
}

IMPLEMENT_SCRIPT(HealthPickup)
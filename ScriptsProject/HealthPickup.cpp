#include "pch.h"
#include "HealthPickup.h"

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

void HealthPickup::OnTriggerEnter(GameObject* gameObject)
{
	//curar


	Pickup::OnTriggerEnter(gameObject);
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

	if (distanceToPlayer <= m_slowDetectionRadius)
	{
		float speed = m_slowSpeed;
		float verticalVelocity = 0.0f;

		if (distanceToPlayer <= m_detectionRadius)
		{
			float closenessFactor = 1.0f - (distanceToPlayer / m_detectionRadius);
			speed = Vector3::Lerp(Vector3(m_slowSpeed, 0, 0), Vector3(m_maxFlySpeed, 0, 0), closenessFactor * closenessFactor).x;

			float targetY = playerPosition.y + 1.0f * closenessFactor;
			verticalVelocity = (targetY - pickupPosition.y) * 2.0f;
		}

		Vector3 directionCopy = directionToPlayer;
		directionCopy.Normalize();

		Vector3 velocity = directionCopy * speed;
		velocity.y = verticalVelocity;

		Transform* transform = GameObjectAPI::getTransform(getOwner());
		Vector3 newPosition = pickupPosition + velocity * dt;
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
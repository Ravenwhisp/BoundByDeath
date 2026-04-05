#include "pch.h"
#include "PlayerRotation.h"

#include <cmath>

static const float PI = 3.1415926535897931f;

static const ScriptFieldInfo playerRotationFields[] =
{
	{ "Turn Speed (deg/s)", ScriptFieldType::Float, offsetof(PlayerRotation, m_turnSpeedDegPerSec), { 0.0f, 2000.0f, 1.0f } }
};

IMPLEMENT_SCRIPT_FIELDS(PlayerRotation, playerRotationFields)

PlayerRotation::PlayerRotation(GameObject* owner)
	: Script(owner)
{
}

void PlayerRotation::Start()
{
	GameObject* owner = getOwner();
	m_initialRotationOffset = TransformAPI::getEulerDegrees(GameObjectAPI::getTransform(owner));
}

void PlayerRotation::Update()
{
}

void PlayerRotation::onAfterDeserialize()
{
	m_yawInitialized = false;
	m_currentYawDeg = 0.0f;
}

void PlayerRotation::applyFacingFromDirection(GameObject* owner, const Vector3& direction, float dt)
{
	const float yawRad = std::atan2(direction.x, direction.z);
	const float targetYawDeg = yawRad * (180.0f / PI);

	if (!m_yawInitialized)
	{
		m_currentYawDeg = 0.0f;
		m_yawInitialized = true;
	}

	const float maxStep = m_turnSpeedDegPerSec * dt;
	m_currentYawDeg = moveTowardsAngleDegrees(m_currentYawDeg, targetYawDeg, maxStep);

	const float finalYaw = wrapAngleDegrees(m_initialRotationOffset.y + m_currentYawDeg);

	TransformAPI::setRotationEuler(GameObjectAPI::getTransform(owner), Vector3(m_initialRotationOffset.x, finalYaw, m_initialRotationOffset.z));
}

float PlayerRotation::wrapAngleDegrees(float angle)
{
	while (angle > 180.0f)
	{
		angle -= 360.0f;
	}
	while (angle < -180.0f)
	{
		angle += 360.0f;
	}
	return angle;
}

float PlayerRotation::moveTowardsAngleDegrees(float currentYawAngle, float targetYawAngle, float maxDelta)
{
	float delta = wrapAngleDegrees(targetYawAngle - currentYawAngle);

	if (delta > maxDelta)
	{
		delta = maxDelta;
	}

	if (delta < -maxDelta)
	{
		delta = -maxDelta;
	}

	return currentYawAngle + delta;
}

IMPLEMENT_SCRIPT(PlayerRotation)
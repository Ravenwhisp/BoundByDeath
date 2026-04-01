#include "pch.h"
#include "EnemyDetectionAggro.h"

static const ScriptFieldInfo enemyDetectionAggroFields[] =
{
	{ "Detection Radius", ScriptFieldType::Float, offsetof(EnemyDetectionAggro, m_detectionRadius), { 0.0f, 50.0f, 0.1f } },
	{ "Lose Aggro Delay", ScriptFieldType::Float, offsetof(EnemyDetectionAggro, m_loseAggroDelay), { 0.0f, 10.0f, 0.1f } },
	{ "Debug Enabled", ScriptFieldType::Bool, offsetof(EnemyDetectionAggro, m_debugEnabled) },

	{ "Player 1 Transform", ScriptFieldType::ComponentRef, offsetof(EnemyDetectionAggro, m_player1Transform), {}, {}, { ComponentType::TRANSFORM } },
	{ "Player 2 Transform", ScriptFieldType::ComponentRef, offsetof(EnemyDetectionAggro, m_player2Transform), {}, {}, { ComponentType::TRANSFORM } }
};

IMPLEMENT_SCRIPT_FIELDS(EnemyDetectionAggro, enemyDetectionAggroFields)

EnemyDetectionAggro::EnemyDetectionAggro(GameObject* owner) : Script(owner) {}

void EnemyDetectionAggro::Start()
{

}

void EnemyDetectionAggro::Update()
{

	// ----- Temporary testing -----//

	Transform* selectedTarget = selectTargetInRange();
	// -----		END		   -----//

	updateAggroState();

}

bool EnemyDetectionAggro::canDetectTarget() const
{
	return false;
}

void EnemyDetectionAggro::enterAggro()
{

}

void EnemyDetectionAggro::exitAggro()
{

}

void EnemyDetectionAggro::updateAggroState()
{

}

Transform* EnemyDetectionAggro::selectTargetInRange() const
{
	const bool player1InRange = isPlayer1InDetectionRange();
	const bool player2InRange = isPlayer2InDetectionRange();

	if (!player1InRange && !player2InRange)
	{
		return nullptr;
	}

	if (player1InRange && !player2InRange)
	{
		return getPlayer1Transform();
	}

	if (!player1InRange && player2InRange)
	{
		return getPlayer2Transform();
	}

	const float distanceToPlayer1 = getDistanceToPlayer1();
	const float distanceToPlayer2 = getDistanceToPlayer2();

	return (distanceToPlayer1 <= distanceToPlayer2) ? getPlayer1Transform() : getPlayer2Transform();
}

// Getters
Transform* EnemyDetectionAggro::getOwnerTransform() const
{
	return GameObjectAPI::getTransform(getOwner());
}

Transform* EnemyDetectionAggro::getPlayer1Transform() const
{
	return m_player1Transform.getReferencedComponent();
}

Transform* EnemyDetectionAggro::getPlayer2Transform() const
{
	return m_player2Transform.getReferencedComponent();
}

Vector3 EnemyDetectionAggro::getOwnerPosition() const
{
	Transform* ownerTransform = getOwnerTransform();
	if (!ownerTransform)
	{
		return Vector3(0.0f, 0.0f, 0.0f);
	}

	return TransformAPI::getPosition(ownerTransform);
}

Vector3 EnemyDetectionAggro::getPlayer1Position() const
{
	Transform* player1Transform = getPlayer1Transform();
	if (!player1Transform)
	{
		return Vector3(0.0f, 0.0f, 0.0f);
	}

	return TransformAPI::getPosition(player1Transform);
}

Vector3 EnemyDetectionAggro::getPlayer2Position() const
{
	Transform* player2Transform = getPlayer2Transform();
	if (!player2Transform)
	{
		return Vector3(0.0f, 0.0f, 0.0f);
	}

	return TransformAPI::getPosition(player2Transform);
}

float EnemyDetectionAggro::getDistanceToPlayer1() const
{
	Vector3 difference = getPlayer1Position() - getOwnerPosition();
	return difference.Length();
}

float EnemyDetectionAggro::getDistanceToPlayer2() const
{
	Vector3 difference = getPlayer2Position() - getOwnerPosition();
	return difference.Length();
}

bool EnemyDetectionAggro::isPlayer1InDetectionRange() const
{
	if (!getPlayer1Transform())
	{
		return false;
	}

	return getDistanceToPlayer1() <= m_detectionRadius;
}

bool EnemyDetectionAggro::isPlayer2InDetectionRange() const
{
	if (!getPlayer2Transform())
	{
		return false;
	}

	return getDistanceToPlayer2() <= m_detectionRadius;
}

IMPLEMENT_SCRIPT(EnemyDetectionAggro)
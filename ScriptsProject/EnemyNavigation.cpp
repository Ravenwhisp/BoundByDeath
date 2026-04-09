#include "pch.h"
#include "EnemyNavigation.h"
#include "EnemyDetectionAggro.h"
#include <cmath>

static const ScriptFieldInfo EnemyNavigationFields[] =
{
	{ "Combat Range", ScriptFieldType::Float, offsetof(EnemyNavigation, m_combatRange), { 0.0f, 50.0f, 0.1f } },
	{ "Move Speed", ScriptFieldType::Float, offsetof(EnemyNavigation, m_moveSpeed), { 0.0f, 50.0f, 0.1f } },
	{ "Turn Speed", ScriptFieldType::Float, offsetof(EnemyNavigation, m_turnSpeed), { 0.0f, 5.0f, 0.1f } },
	{ "Interval", ScriptFieldType::Float, offsetof(EnemyNavigation, m_intervalRepath), { 0.0f, 50.0f, 0.1f } },
	{ "Debug Enabled", ScriptFieldType::Bool, offsetof(EnemyNavigation, m_debugEnabled) }
};

IMPLEMENT_SCRIPT_FIELDS(EnemyNavigation, EnemyNavigationFields)

EnemyNavigation::EnemyNavigation(GameObject* owner)
    : Script(owner)
{
}

void EnemyNavigation::Start()
{

	Script* script = GameObjectAPI::getScript(m_owner, "EnemyDetectionAggro");
	m_enemyDetectionAggro = dynamic_cast<EnemyDetectionAggro*>(script);

	if (!m_enemyDetectionAggro)
	{
		Debug::error("EnemyDetectionAggro script not found!");
	}
}

void EnemyNavigation::Update()
{

	if (!m_enemyDetectionAggro)
	{
		m_currentState = NavigationState::Idle;
		m_currentTarget = nullptr;
		updateIdle();
		return;
	}

	if (hasValidTarget())
	{
		m_currentTarget = m_enemyDetectionAggro->getCurrentTarget();

		if (isTargetInCombatRange())
		{
			m_currentState = NavigationState::Idle;
			updateIdle();
		}
		else
		{
			m_currentState = NavigationState::Chase;
			updateChase();
		}
	}
	else
	{
		m_currentTarget = nullptr;
		m_currentState = NavigationState::Idle;
		updateIdle();
		return;
	}
}

void EnemyNavigation::drawGizmo()
{
	if (!m_debugEnabled)
	{
		return;
	}

	if (m_path.size() < 2)
	{
		return;
	}

	const Vector3 cyan = { 0.0f, 1.0f, 1.0f };
	for (int i = 0; i < m_path.size() - 1; ++i)
	{
		DebugDrawAPI::drawLine(m_path[i], m_path[i + 1], cyan, 0, true);
	}
}

bool EnemyNavigation::hasValidTarget() const
{
	if (m_enemyDetectionAggro->getCurrentTarget())
	{
		return true;
	}

	return false;
}

bool EnemyNavigation::isTargetInCombatRange() const
{
	if (!m_currentTarget)
	{
		return false;
	}

	Vector3 distance = m_owner->GetTransform()->getPosition() - m_currentTarget->getPosition();

	if (distance.Length() <= m_combatRange)
	{
		return true;
	}

	return false;
}

void EnemyNavigation::clearPath()
{
	m_path.clear();
	m_hasPath = false;
	m_currentIndex = 0;
}

bool EnemyNavigation::buildPathToTarget()
{
	if (!m_currentTarget)
	{
		return false;
	}

	Vector3 start = m_owner->GetTransform()->getPosition();
	Vector3 end = getChasePosition();

	std::vector<Vector3> tempPath;
	tempPath.resize(m_maxPathPoints);

	int pointCount = NavigationAPI::findStraightPath(start, end, tempPath.data(), m_maxPathPoints, m_searchExtents);

	if (pointCount > 0)
	{
		clearPath();

		for (int i = 0; i < pointCount; ++i)
		{
			m_path.push_back(tempPath[i]);
		}

		// need to check if first point is current position or not
		m_currentIndex = 0;
		m_hasPath = true;
		return true;
	}
	
	clearPath();

	return false;
}

void EnemyNavigation::followPath()
{
	if (!m_hasPath)
	{
		return;
	}

	if (m_currentIndex >= m_path.size())
	{
		clearPath();
		return;
	}

	Vector3 currentPosition = m_owner->GetTransform()->getPosition();
	Vector3 targetPoint = m_path[m_currentIndex];
	Vector3 direction = targetPoint - currentPosition;

	float distance = direction.Length();

	if (distance < 0.1f)
	{
		m_currentIndex++;
		if (m_currentIndex >= m_path.size())
		{
			clearPath();
			return;
		}
		return;
	}

	direction.Normalize();
	rotateTowardsDirection(direction);
	float dt = Time::getDeltaTime();

	currentPosition += direction * m_moveSpeed * dt;

	TransformAPI::setPosition(m_owner->GetTransform(), currentPosition);	
}

Vector3 EnemyNavigation::getChasePosition() const
{
	if (!m_currentTarget)
	{
		return m_owner->GetTransform()->getPosition();
	}

	Vector3 ownerPos = m_owner->GetTransform()->getPosition();
	Vector3 targetPos = m_currentTarget->getPosition();
	Vector3 direction = targetPos - ownerPos;

	float distance = direction.Length();

	if (distance < 0.001f)
	{
		return targetPos;
	}

	direction.Normalize();

	Vector3 chasePosition = targetPos - direction * m_combatRange;

	return chasePosition;
}

void EnemyNavigation::updateIdle()
{
	m_repathTimer = 0.0f;
	if (m_hasPath)
	{
		clearPath();
	}
}

void EnemyNavigation::updateChase()
{
	if (!m_hasPath)
	{
		if (!buildPathToTarget())
		{
			return;
		}
		m_repathTimer = 0.0f;
	}

	m_repathTimer += Time::getDeltaTime();

	if (m_repathTimer >= m_intervalRepath)
	{
		if (!buildPathToTarget())
		{
			return;
		}
		m_repathTimer = 0.0f;
	}

	if (m_hasPath)
	{
		followPath();
	}
}

void EnemyNavigation::rotateTowardsDirection(const Vector3& direction)
{
	Vector3 desiredDirection = direction;
	desiredDirection.y = 0.0f;

	if (desiredDirection.LengthSquared() < 0.0001f)
	{
		return;
	}

	Vector3 currentForward = TransformAPI::getForward(m_owner->GetTransform());
	currentForward.y = 0.0f;

	if (currentForward.LengthSquared() < 0.0001f)
	{
		return;
	}

	desiredDirection.Normalize();
	currentForward.Normalize();

	float dot = currentForward.Dot(desiredDirection);

	if (dot > 1.0f) dot = 1.0f;
	if (dot < -1.0f) dot = -1.0f;

	float angle = std::acos(dot);

	Vector3 cross = currentForward.Cross(desiredDirection);

	float sign = (cross.y > 0) ? 1.0f : -1.0f;

	Vector3 currentEulerRotation = TransformAPI::getEulerDegrees(m_owner->GetTransform());
	float maxStep = (m_turnSpeed * 100) * Time::getDeltaTime();

	float step = 0.0f;
	float angleDeg = angle * RADIANS_TO_DEGREES;
	if (angleDeg > maxStep)
	{
		step = maxStep * sign;
	}
	else
	{
		step = angleDeg * sign;
	}

	currentEulerRotation.y += step;
	TransformAPI::setRotationEuler(m_owner->GetTransform(), currentEulerRotation);
}

IMPLEMENT_SCRIPT(EnemyNavigation)
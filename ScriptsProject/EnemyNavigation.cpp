#include "pch.h"
#include "EnemyNavigation.h"
#include "EnemyDetectionAggro.h"

static const ScriptFieldInfo EnemyNavigationFields[] =
{
	{ "Combat Range", ScriptFieldType::Float, offsetof(EnemyNavigation, m_combatRange), { 0.0f, 50.0f, 0.1f } },
	{ "Move Speed", ScriptFieldType::Float, offsetof(EnemyNavigation, m_moveSpeed), { 0.0f, 50.0f, 0.1f } },
	{ "Turn Speed", ScriptFieldType::Float, offsetof(EnemyNavigation, m_turnSpeed), { 0.0f, 50.0f, 0.1f } },
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

	if (Input::isKeyDown(KeyCode::Num1))
	{
		if (m_currentState == NavigationState::Idle)
			Debug::log("Idle");
		else if (m_currentState == NavigationState::Chase)
		{
			Debug::log("Chase");
			if (!m_hasPath)
			{
				if (buildPathToTarget())
					Debug::log("Path built");
			}			
		}
		else
			Debug::log("Error");
	}

	if (!m_enemyDetectionAggro)
	{
		m_currentState = NavigationState::Idle;
		m_currentTarget = nullptr;
		return;
	}

	if (hasValidTarget())
	{
		m_currentTarget = m_enemyDetectionAggro->getCurrentTarget();

		if (isTargetInCombatRange())
		{
			m_currentState = NavigationState::Idle;
		}
		else
		{
			m_currentState = NavigationState::Chase;
			if (m_hasPath)
				followPath();
		}
	}
	else
	{
		m_currentTarget = nullptr;
		m_currentState = NavigationState::Idle;
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

IMPLEMENT_SCRIPT(EnemyNavigation)
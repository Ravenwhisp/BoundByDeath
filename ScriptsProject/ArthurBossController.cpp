#include "pch.h"
#include "ArthurBossController.h"
#include "ArthurDetectionAggro.h"
#include "Damageable.h"
#include <cmath>

static const char* navAgentProfileNames[] =
{
	"PlayerNormal",
	"PlayerSpectral",
	"EnemyGround"
};

constexpr int navAgentProfileCount = 3;

IMPLEMENT_SCRIPT_FIELDS(ArthurBossController,
	SERIALIZED_ENUM_INT(m_enemyType, "Enemy Type", navAgentProfileNames, navAgentProfileCount),
	SERIALIZED_FLOAT(m_combatRange, "Combat Range", 0.0f, 50.0f, 0.1f),
	SERIALIZED_FLOAT(m_moveSpeed, "Move Speed", 0.0f, 50.0f, 0.1f),
	SERIALIZED_FLOAT(m_turnSpeed, "Turn Speed", 0.0f, 5.0f, 0.1f),
	SERIALIZED_FLOAT(m_intervalRepath, "Interval", 0.0f, 50.0f, 0.1f),
	SERIALIZED_BOOL(m_debugEnabled, "Debug Enabled")
)

ArthurBossController::ArthurBossController(GameObject* owner)
	: Script(owner)
{
}

void ArthurBossController::Start()
{
	m_arthurDetectionAggro = GameObjectAPI::findScript<ArthurDetectionAggro>(getOwner());

	if (!m_arthurDetectionAggro)
	{
		Debug::error("ArthurDetectionAggro script not found!");
	}
}

void ArthurBossController::drawGizmo()
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

bool ArthurBossController::hasValidTarget() const
{
	if (!m_arthurDetectionAggro)
	{
		return false;
	}

	Transform* targetTransform = m_arthurDetectionAggro->getCurrentTarget();
	if (!targetTransform)
	{
		return false;
	}

	GameObject* targetObject = ComponentAPI::getOwner(targetTransform);
	if (!targetObject)
	{
		return false;
	}

	Damageable* damageable = GameObjectAPI::findScript<Damageable>(targetObject);
	if (damageable && damageable->isDead())
	{
		return false;
	}

	return true;
}

void ArthurBossController::updateCurrentTarget()
{
	if (!m_arthurDetectionAggro)
	{
		m_arthurDetectionAggro = GameObjectAPI::findScript<ArthurDetectionAggro>(getOwner());
	}

	if (!m_arthurDetectionAggro)
	{
		m_currentTarget = nullptr;
		return;
	}

	m_currentTarget = m_arthurDetectionAggro->getCurrentTarget();
}

bool ArthurBossController::isTargetInCombatRange() const
{
	if (!m_currentTarget)
	{
		return false;
	}

	Vector3 ownerPosition = getOwner()->GetTransform()->getPosition();
	Vector3 targetPosition = getOwner()->GetTransform()->getPosition();

	Vector3 difference = ownerPosition - targetPosition;
	difference.y = 0.0f;

	return difference.Length() <= m_combatRange;
}

void ArthurBossController::clearPath()
{
	m_path.clear();
	m_hasPath = false;
	m_currentIndex = 0;
}

bool ArthurBossController::buildPathToTarget()
{
	if (!m_currentTarget)
	{
		return false;
	}

	Vector3 start = getOwner()->GetTransform()->getPosition();
	Vector3 end = getChasePosition();

	std::vector<Vector3> tempPath;
	tempPath.resize(m_maxPathPoints);

	int pointCount = NavigationAPI::findStraightPath(start, end, tempPath.data(), m_maxPathPoints, m_searchExtents, static_cast<NavAgentProfile>(m_enemyType));

	if (pointCount > 0)
	{
		clearPath();

		for (int i = 0; i < pointCount; ++i)
		{
			m_path.push_back(tempPath[i]);
		}

		m_currentIndex = 0;
		m_hasPath = true;
		return true;
	}

	clearPath();

	return false;
}

void ArthurBossController::followPath()
{
	//todo
}

Vector3 ArthurBossController::getChasePosition() const
{
	// todo
	Vector3 chasePosition;
	return chasePosition;
}

void ArthurBossController::rotateTowardsDirection(const Vector3& direction)
{
	// todo
}

void ArthurBossController::faceCurrentTarget()
{
	// todo
}

void ArthurBossController::resetRepathTimer()
{
	m_repathTimer = 0.0f;
}

void ArthurBossController::addToRepathTimer(float dt)
{
	m_repathTimer += dt;
}

bool ArthurBossController::shouldRepath() const
{
	return m_repathTimer >= m_intervalRepath;
}

IMPLEMENT_SCRIPT(ArthurBossController)
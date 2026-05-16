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

}

bool ArthurBossController::hasValidTarget() const
{
	return true;
}

void ArthurBossController::updateCurrentTarget()
{

}

bool ArthurBossController::isTargetInCombatRange() const
{
	return true;
}

void ArthurBossController::clearPath()
{
	//todo
}

bool ArthurBossController::buildPathToTarget()
{
	//todo
	return true;
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
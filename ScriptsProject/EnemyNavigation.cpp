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
	Script* script = GameObjectAPI::getScript(getOwner(), "EnemyDetectionAggro");
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
			Debug::log("Chase");
		else
			Debug::log("Error");
	}

	if (!m_enemyDetectionAggro)
	{
		m_currentState = NavigationState::Idle;
		return;
	}

	m_currentTarget = m_enemyDetectionAggro->getCurrentTarget();
	if (!m_currentTarget)
	{
		m_currentState = NavigationState::Idle;
		return;
	}

	Vector3 distance = getOwner()->GetTransform()->getPosition() - m_currentTarget->getPosition();

	if (distance.Length() <= m_combatRange)
	{
		m_currentState = NavigationState::Idle;
	}
	else
	{
		m_currentState = NavigationState::Chase;
	}


}

IMPLEMENT_SCRIPT(EnemyNavigation)
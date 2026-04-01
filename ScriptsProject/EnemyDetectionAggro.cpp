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

IMPLEMENT_SCRIPT(EnemyDetectionAggro)
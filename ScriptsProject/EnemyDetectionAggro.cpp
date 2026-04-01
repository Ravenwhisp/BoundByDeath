#include "pch.h"
#include "EnemyDetectionAggro.h"

static const ScriptFieldInfo enemyDetectionAggroFields[] =
{
	{ "Detection Radius", ScriptFieldType::Float, offsetof(EnemyDetectionAggro, m_detectionRadius), { 0.0f, 50.0f, 0.1f } },
	{ "Lose Aggro Delay", ScriptFieldType::Float, offsetof(EnemyDetectionAggro, m_loseAggroDelay), { 0.0f, 10.0f, 0.1f } },
	{ "Debug Enabled", ScriptFieldType::Bool, offsetof(EnemyDetectionAggro, m_debugEnabled) }
};

IMPLEMENT_SCRIPT_FIELDS(EnemyDetectionAggro, enemyDetectionAggroFields)

EnemyDetectionAggro::EnemyDetectionAggro(GameObject* owner) : Script(owner) {}

void EnemyDetectionAggro::Start()
{

}

void EnemyDetectionAggro::Update()
{

}

IMPLEMENT_SCRIPT(EnemyDetectionAggro)
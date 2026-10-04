#include "pch.h"
#include "RockyEnemyController.h"

#include "EnemyDetectionAggro.h"
#include "EnemyBaseAttackConfig.h"

#include "Damageable.h"

RockyEnemyController::RockyEnemyController(GameObject* owner)
	: EnemyBaseController(owner)
{
}

void RockyEnemyController::Start()
{
	EnemyBaseController::Start();

	m_enemyDetectionAggro = GameObjectAPI::findScript<EnemyDetectionAggro>(getOwner());

	if (!m_enemyDetectionAggro)
	{
		Debug::warn("[RockyEnemyController] EnemyDetectionAggro not found on '%s'.", GameObjectAPI::getName(getOwner()));
	}

	m_currentTarget = nullptr;
	m_deathTriggerSent = false;

	resetRepathTimer();
	clearPath();
}

void RockyEnemyController::Update()
{
	updateCurrentTarget();
}

Transform* RockyEnemyController::acquireCurrentTarget()
{
	if (!m_enemyDetectionAggro)
	{
		m_enemyDetectionAggro = GameObjectAPI::findScript<EnemyDetectionAggro>(getOwner());
	}

	if (!m_enemyDetectionAggro)
	{
		return nullptr;
	}

	return m_enemyDetectionAggro->getCurrentTarget();
}

bool RockyEnemyController::isTargetDowned(Transform* target) const
{
	if (!m_enemyDetectionAggro || !target)
	{
		return true;
	}

	return m_enemyDetectionAggro->isDowned(target);
}

const EnemyBaseAttackConfig* RockyEnemyController::getAttackConfig() const
{
	return m_attackConfig.get();
}

IMPLEMENT_SCRIPT_FIELDS_INHERITED(RockyEnemyController, EnemyBaseController,
	SERIALIZED_ASSET_REF(m_attackConfig, "Attack Config", AssetType::DATA_CONTAINER)
)

IMPLEMENT_SCRIPT(RockyEnemyController)
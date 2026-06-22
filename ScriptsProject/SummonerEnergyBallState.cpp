#include "pch.h"
#include "SummonerEnergyBallState.h"

#include "SummonerEnemyController.h"
#include "SummonerAttackConfig.h"
#include "EnergyBallProjectile.h"

#include "Damageable.h"
#include "PlayerState.h"

SummonerEnergyBallState::SummonerEnergyBallState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void SummonerEnergyBallState::OnStateEnter()
{
	m_controller = GameObjectAPI::findScript<SummonerEnemyController>(getOwner());
	m_attackConfig = GameObjectAPI::findScript<SummonerAttackConfig>(getOwner());
	m_animation = AnimationAPI::getAnimationComponent(getOwner());

	m_stateTimer = 0.0f;
	m_hasFiredEnergyBall = false;

	if (!m_controller)
	{
		Debug::error("[SummonerEnergyBallState] EnemyController not found.");
		return;
	}

	if (!m_attackConfig)
	{
		Debug::error("[SummonerEnergyBallState] AttackConfig not found.");
		return;
	}

	if (!m_animation)
	{
		Debug::error("[SummonerEnergyBallState] AnimationComponent not found.");
		return;
	}

	m_controller->updateCurrentTarget();
	m_committedTarget = m_controller->getCurrentTarget();

	Debug::log("[SummonerEnergyBallState] ENTER");
}

void SummonerEnergyBallState::OnStateUpdate()
{
	if (!m_controller || !m_attackConfig || !m_animation)
	{
		return;
	}

	if (m_controller->trySendDeathTrigger(m_animation))
	{
		return;
	}

	m_controller->faceCurrentTarget();

	m_stateTimer += Time::getDeltaTime();

	if (!m_hasFiredEnergyBall && m_stateTimer >= m_attackConfig->m_basicAttackWindupTime)
	{
		spawnEnergyBall();
		m_hasFiredEnergyBall = true;
	}

	if (m_stateTimer >= m_attackConfig->m_basicAttackTotalDuration)
	{
		AnimationAPI::sendTrigger(m_animation, "ToIdle");
		return;
	}
}

void SummonerEnergyBallState::OnStateExit()
{
	m_stateTimer = 0.0f;
	m_hasFiredEnergyBall = false;

	Debug::log("[SummonerEnergyBallState] EXIT");
}

void SummonerEnergyBallState::spawnEnergyBall()
{
	if (!m_controller || !m_attackConfig)
	{
		return;
	}

	Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
	Transform* targetTransform = m_controller->getCurrentTarget();

	if (!ownerTransform || !targetTransform)
	{
		return;
	}

	const Vector3 ownerPosition = TransformAPI::getPosition(ownerTransform);
	const Vector3 targetPosition = TransformAPI::getPosition(targetTransform);

	Vector3 direction = targetPosition - ownerPosition;
	direction.y = 0.0f;

	if (direction.LengthSquared() <= 0.00001f)
	{
		return;
	}

	direction.Normalize();

	const Vector3 spawnPosition = ownerPosition + direction * m_attackConfig->m_energyBallSpawnOffset;

	GameObject* projectileObject = GameObjectAPI::instantiatePrefab(
		m_attackConfig->m_energyBallPrefabPath.c_str(),
		spawnPosition,
		Vector3::Zero
	);

	if (!projectileObject)
	{
		return;
	}

	EnergyBallProjectile* projectile = GameObjectAPI::findScript<EnergyBallProjectile>(projectileObject);
	if (!projectile)
	{
		Debug::error("[SummonerEnergyBallState] EnergyBallProjectile script not found.");
		return;
	}

	projectile->launch(
		direction,
		m_attackConfig->m_energyBallSpeed,
		m_attackConfig->m_basicAttackDamage
	);

	Debug::log("[SummonerEnergyBallState] Energy Ball spawned");
}

IMPLEMENT_SCRIPT(SummonerEnergyBallState)
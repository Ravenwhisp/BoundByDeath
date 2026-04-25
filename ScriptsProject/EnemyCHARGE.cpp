#include "pch.h"
#include "EnemyCHARGE.h"
#include "EnemyController.h"

static const ScriptFieldInfo CHARGEFields[] =
{
	{ "Charge Duration", ScriptFieldType::Float, offsetof(EnemyCHARGE, m_chargeDuration), { 0.0f, 10.0f, 0.1f } },
	{ "Charge Speed", ScriptFieldType::Float, offsetof(EnemyCHARGE, m_chargeSpeed), { 0.0f, 50.0f, 0.1f } },
	{ "Charge Cooldown", ScriptFieldType::Float, offsetof(EnemyCHARGE, m_chargeCooldown), { 0.0f, 20.0f, 0.1f } },
	{ "Debug Enabled", ScriptFieldType::Bool, offsetof(EnemyCHARGE, m_debugEnabled) }
};

IMPLEMENT_SCRIPT_FIELDS(EnemyCHARGE, CHARGEFields)

EnemyCHARGE::EnemyCHARGE(GameObject* owner)
	: StateMachineScript(owner)
{
}

void EnemyCHARGE::OnStateEnter()
{
	m_elapsedTime = 0.0f;

	Script* script = GameObjectAPI::getScript(getOwner(), "EnemyController");
	m_enemyController = dynamic_cast<EnemyController*>(script);

	Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
	if (!ownerTransform)
	{
		return;
	}

	if (m_enemyController)
	{
		m_enemyController->consumeChargeCooldown(m_chargeCooldown);
		m_enemyController->clearPath();
		m_enemyController->resetRepathTimer();
		m_enemyController->updateCurrentTarget();
	}

	Vector3 ownerPosition = TransformAPI::getPosition(ownerTransform);
	Vector3 direction = TransformAPI::getForward(ownerTransform);

	if (m_enemyController && m_enemyController->hasValidTarget() && m_enemyController->getCurrentTarget())
	{
		Vector3 targetPosition = TransformAPI::getPosition(m_enemyController->getCurrentTarget());
		direction = targetPosition - ownerPosition;
	}

	direction.y = 0.0f;

	if (direction.LengthSquared() > 0.0001f)
	{
		direction.Normalize();
		m_chargeDirection = direction;
	}

	if (m_debugEnabled)
	{
		Debug::log("[EnemyCHARGE] ENTER");
	}
}

void EnemyCHARGE::OnStateUpdate()
{
	AnimationComponent* animation = AnimationAPI::getAnimationComponent(getOwner());
	if (!animation)
	{
		return;
	}

	Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
	if (!ownerTransform)
	{
		return;
	}

	if (!m_enemyController)
	{
		AnimationAPI::playState(animation, "Idle");
		return;
	}

	m_enemyController->updateCurrentTarget();

	if (!m_enemyController->hasValidTarget())
	{
		AnimationAPI::playState(animation, "Idle");
		return;
	}

	if (m_enemyController->isTargetInCombatRange())
	{
		m_enemyController->faceCurrentTarget();
		AnimationAPI::playState(animation, "Attack");
		return;
	}

	Vector3 ownerPosition = TransformAPI::getPosition(ownerTransform);
	ownerPosition += m_chargeDirection * m_chargeSpeed * Time::getDeltaTime();
	TransformAPI::setPosition(ownerTransform, ownerPosition);

	m_elapsedTime += Time::getDeltaTime();

	m_enemyController->updateCurrentTarget();

	if (m_enemyController->isTargetInCombatRange())
	{
		m_enemyController->faceCurrentTarget();
		AnimationAPI::playState(animation, "Attack");
		return;
	}

	if (m_elapsedTime >= m_chargeDuration)
	{
		AnimationAPI::playState(animation, "Chase");
		return;
	}
}

void EnemyCHARGE::OnStateExit()
{
	if (m_debugEnabled)
	{
		Debug::log("[EnemyCHARGE] EXIT");
	}
}

IMPLEMENT_SCRIPT(EnemyCHARGE)
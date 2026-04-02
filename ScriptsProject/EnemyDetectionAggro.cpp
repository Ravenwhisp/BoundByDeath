#include "pch.h"
#include "EnemyDetectionAggro.h"

static const ScriptFieldInfo enemyDetectionAggroFields[] =
{
	{ "Detection Radius", ScriptFieldType::Float, offsetof(EnemyDetectionAggro, m_detectionRadius), { 0.0f, 50.0f, 0.1f } },
	{ "Lose Aggro Delay", ScriptFieldType::Float, offsetof(EnemyDetectionAggro, m_loseAggroDelay), { 0.0f, 10.0f, 0.1f } },
	{ "Target Lock Duration", ScriptFieldType::Float, offsetof(EnemyDetectionAggro, m_targetLockDuration), { 0.0f, 10.0f, 0.1f } },
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
	m_currentTime += Time::getDeltaTime();

	updateTargetLockTimer();
	updateAggroState();
}

void EnemyDetectionAggro::drawGizmo()
{
	if (!m_debugEnabled)
	{
		return;
	}

	const Vector3 white = { 1.0f, 1.0f, 1.0f };
	const Vector3 red = { 1.0f, 0.0f, 0.0f };
	const Vector3 cyan = { 0.0f, 1.0f, 1.0f };

	Vector3 debugPosition = getOwnerPosition() + Vector3(0.0f, 0.2f, 0.0f);

	DebugDrawAPI::drawCircle(debugPosition, Vector3(0.0f, 1.0f, 0.0f), white, m_detectionRadius, 24.0f, 0, true);

	if (m_currentTargetTransform)
	{
		Vector3 targetPosition = TransformAPI::getPosition(m_currentTargetTransform);
		DebugDrawAPI::drawLine(debugPosition, targetPosition, red, 0, true);
	}

	if (m_isAggro)
	{
		DebugDrawAPI::drawCross(m_lastKnownTargetPosition, 0.35f, 0, true);
		DebugDrawAPI::drawPoint(m_lastKnownTargetPosition, cyan, 4.0f, 0, true);
	}
}

void EnemyDetectionAggro::enterAggro(Transform* target)
{
	if (!target)
	{
		return;
	}

	m_isAggro = true;
	m_canSeeTarget = true;
	m_currentTargetTransform = target;
	m_timeSinceLastSeen = 0.0f;
	m_lastKnownTargetPosition = TransformAPI::getPosition(target);

	startTargetLock();
}

void EnemyDetectionAggro::exitAggro()
{
	m_isAggro = false;
	m_canSeeTarget = false;
	m_currentTargetTransform = nullptr;
	m_timeSinceLastSeen = 0.0f;
	m_currentTargetLockTimer = 0.0f;
}

void EnemyDetectionAggro::updateAggroState()
{
	updateAggroEntries();
	updateAggroScores();

	Transform* detectedTarget = selectBestAggroTarget();

	if (detectedTarget)
	{
		if (m_currentTargetTransform && isTargetLockActive())
		{
			const bool currentTargetStillDetected =
				(m_currentTargetTransform == getPlayer1Transform() && isPlayer1InDetectionRange()) ||
				(m_currentTargetTransform == getPlayer2Transform() && isPlayer2InDetectionRange());

			if (currentTargetStillDetected)
			{
				m_isAggro = true;
				m_canSeeTarget = true;
				m_timeSinceLastSeen = 0.0f;
				m_lastKnownTargetPosition = TransformAPI::getPosition(m_currentTargetTransform);
				return;
			}
		}
		
		if (!m_currentTargetTransform)
		{
			enterAggro(detectedTarget);
			return;
		}

		float currentScore = getAggroScore(m_currentTargetTransform);
		float newScore = getAggroScore(detectedTarget);

		if (detectedTarget != m_currentTargetTransform && newScore > currentScore + m_targetSwitchThreshold)
		{
			enterAggro(detectedTarget);
			return;
		}

		m_isAggro = true;
		m_canSeeTarget = true;
		m_timeSinceLastSeen = 0.0f;
		m_lastKnownTargetPosition = TransformAPI::getPosition(m_currentTargetTransform);
		return;
	}

	m_canSeeTarget = false;

	if (m_isAggro)
	{
		m_timeSinceLastSeen += Time::getDeltaTime();

		if (m_timeSinceLastSeen >= m_loseAggroDelay)
		{
			exitAggro();
		}
	}
}

void EnemyDetectionAggro::updateAggroScores()
{
	m_player1Aggro.aggroScore = calculateAggroScore(m_player1Aggro);
	m_player2Aggro.aggroScore = calculateAggroScore(m_player2Aggro);
}

void EnemyDetectionAggro::updateAggroEntries()
{
	Transform* player1 = getPlayer1Transform();
	Transform* player2 = getPlayer2Transform();

	m_player1Aggro.targetTransform = player1;
	m_player2Aggro.targetTransform = player2;

	m_player1Aggro.isInDetectionRange = isPlayer1InDetectionRange();
	m_player2Aggro.isInDetectionRange = isPlayer2InDetectionRange();

	m_player1Aggro.distanceToEnemy = getDistanceToPlayer1();
	m_player2Aggro.distanceToEnemy = getDistanceToPlayer2();
}

float EnemyDetectionAggro::calculateAggroScore(const AggroEntry& entry) const
{
	if (!entry.targetTransform || !entry.isInDetectionRange)
	{
		return -9999.9f;
	}

	float score = 0.0f;

	score += (1.0f / (entry.distanceToEnemy + 0.1f)) * m_distanceWeight * 10.0f;

	float timeSinceAttack = m_currentTime - entry.lastAttackTime;
	if (timeSinceAttack <= m_recentAttackMemory)
	{
		score += m_recentAttackBonus;
	}

	float timeSinceDamage = m_currentTime - entry.lastDamageTime;
	if (timeSinceDamage <= m_recentDamageMemory)
	{
		score += m_recentDamageBonus;
	}

	return score;
}

bool EnemyDetectionAggro::isTargetLockActive() const
{
	return m_currentTargetLockTimer > 0.0f;
}

void EnemyDetectionAggro::startTargetLock()
{
	m_currentTargetLockTimer = m_targetLockDuration;
}

void EnemyDetectionAggro::updateTargetLockTimer()
{
	if (isTargetLockActive())
	{
		m_currentTargetLockTimer -= Time::getDeltaTime();

		if (m_currentTargetLockTimer < 0.0f)
		{
			m_currentTargetLockTimer = 0.0f;
		}
	}
}

Transform* EnemyDetectionAggro::selectBestAggroTarget() const
{
	const AggroEntry* bestEntry = nullptr;

	Transform* player1 = m_player1Aggro.targetTransform;
	Transform* player2 = m_player2Aggro.targetTransform;

	if (player1 && getAggroScore(player1) > -9999.9f)
	{
		bestEntry = &m_player1Aggro;
	}

	if (player2 && getAggroScore(player2) > -9999.9f)
	{
		if (!bestEntry || getAggroScore(player2) > bestEntry->aggroScore)
		{
			bestEntry = &m_player2Aggro;
		}
	}

	if (!bestEntry)
	{
		return nullptr;
	}

	return bestEntry->targetTransform;
}

void EnemyDetectionAggro::notifyPlayerAttackedEnemy(Transform* playerTransform)
{
	AggroEntry* entry = getAggroEntry(playerTransform);
	
	if (!entry)
	{
		return;
	}

	entry->lastAttackTime = m_currentTime;

	if (!m_isAggro)
	{
		enterAggro(playerTransform);
		return;
	}
}

void EnemyDetectionAggro::notifyPlayerDealtDamage(Transform* playerTransform)
{
	AggroEntry* entry = getAggroEntry(playerTransform);
	
	if (!entry)
	{
		return;
	}

	entry->lastDamageTime = m_currentTime;

	if (!m_isAggro)
	{
		enterAggro(playerTransform);
		return;
	}
}

Transform* EnemyDetectionAggro::getOwnerTransform() const
{
	return GameObjectAPI::getTransform(getOwner());
}

Transform* EnemyDetectionAggro::getPlayer1Transform() const
{
	return m_player1Transform.getReferencedComponent();
}

Transform* EnemyDetectionAggro::getPlayer2Transform() const
{
	return m_player2Transform.getReferencedComponent();
}

Vector3 EnemyDetectionAggro::getOwnerPosition() const
{
	Transform* ownerTransform = getOwnerTransform();
	if (!ownerTransform)
	{
		return Vector3(0.0f, 0.0f, 0.0f);
	}

	return TransformAPI::getPosition(ownerTransform);
}

Vector3 EnemyDetectionAggro::getPlayer1Position() const
{
	Transform* player1Transform = getPlayer1Transform();
	if (!player1Transform)
	{
		return Vector3(0.0f, 0.0f, 0.0f);
	}

	return TransformAPI::getPosition(player1Transform);
}

Vector3 EnemyDetectionAggro::getPlayer2Position() const
{
	Transform* player2Transform = getPlayer2Transform();
	if (!player2Transform)
	{
		return Vector3(0.0f, 0.0f, 0.0f);
	}

	return TransformAPI::getPosition(player2Transform);
}

float EnemyDetectionAggro::getDistanceToPlayer1() const
{
	Vector3 difference = getPlayer1Position() - getOwnerPosition();
	return difference.Length();
}

float EnemyDetectionAggro::getDistanceToPlayer2() const
{
	Vector3 difference = getPlayer2Position() - getOwnerPosition();
	return difference.Length();
}

bool EnemyDetectionAggro::isPlayer1InDetectionRange() const
{
	if (!getPlayer1Transform())
	{
		return false;
	}

	return getDistanceToPlayer1() <= m_detectionRadius;
}

bool EnemyDetectionAggro::isPlayer2InDetectionRange() const
{
	if (!getPlayer2Transform())
	{
		return false;
	}

	return getDistanceToPlayer2() <= m_detectionRadius;
}

EnemyDetectionAggro::AggroEntry* EnemyDetectionAggro::getAggroEntry(Transform* target)
{
	if (target == getPlayer1Transform())
	{
		return &m_player1Aggro;
	}
	if (target == getPlayer2Transform())
	{
		return &m_player2Aggro;
	}
	return nullptr;
}

const EnemyDetectionAggro::AggroEntry* EnemyDetectionAggro::getAggroEntry(Transform* target) const
{
	if (target == getPlayer1Transform())
	{
		return &m_player1Aggro;
	}
	if (target == getPlayer2Transform())
	{
		return &m_player2Aggro;
	}
	return nullptr;
}

float EnemyDetectionAggro::getAggroScore(Transform* target) const
{
	const AggroEntry* entry = getAggroEntry(target);

	if (!entry)
	{
		return -9999.9f;
	}

	return entry->aggroScore;
}

IMPLEMENT_SCRIPT(EnemyDetectionAggro)
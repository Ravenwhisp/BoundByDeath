#include "pch.h"
#include "ArthurDetectionAggro.h"
#include "Damageable.h"
#include "DeathCharacter.h" 

IMPLEMENT_SCRIPT_FIELDS(ArthurDetectionAggro,
	SERIALIZED_BOOL(m_encounterStarted, "Start Encounter"),
	SERIALIZED_FLOAT(m_detectionRadius, "Detection Radius", 0.0f, 50.0f, 0.1f),
	SERIALIZED_FLOAT(m_targetLockDuration, "Target Lock Duration", 0.0f, 10.0f, 0.1f),
	SERIALIZED_BOOL(m_debugEnabled, "Debug Enabled"),
	SERIALIZED_COMPONENT_REF(m_lyrielTransform, "Lyriel Transform", ComponentType::TRANSFORM),
	SERIALIZED_COMPONENT_REF(m_deathTransform, "Death Transform", ComponentType::TRANSFORM)
)

static bool isDeadTarget(Transform* targetTransform)
{
	if (targetTransform == nullptr)
	{
		return false;
	}

	GameObject* targetObject = ComponentAPI::getOwner(targetTransform);
	if (targetObject == nullptr)
	{
		return false;
	}

	Damageable* damageable = GameObjectAPI::findScript<Damageable>(targetObject);
	if (damageable == nullptr)
	{
		return false;
	}

	return damageable->isDead();
}

static bool isValidAliveTarget(Transform* targetTransform)
{
	return targetTransform && !isDeadTarget(targetTransform);
}

ArthurDetectionAggro::ArthurDetectionAggro(GameObject* owner) : Script(owner) {}

void ArthurDetectionAggro::Start()
{
}

void ArthurDetectionAggro::Update()
{
	m_currentTime += Time::getDeltaTime();

	updateTargetLockTimer();
	updateTauntTimer();
	updateAggroState();
}

void ArthurDetectionAggro::drawGizmo()
{
	if (!m_debugEnabled)
	{
		return;
	}

	const Vector3 white = { 1.0f, 1.0f, 1.0f };
	const Vector3 red = { 1.0f, 0.0f, 0.0f };
	const Vector3 yellow = { 1.0f, 1.0f, 0.0f };
	const Vector3 cyan = { 0.0f, 1.0f, 1.0f };

	Vector3 debugPosition = getOwnerPosition() + Vector3(0.0f, 0.2f, 0.0f);

	DebugDrawAPI::drawCircle(debugPosition, Vector3(0.0f, 1.0f, 0.0f), white, m_detectionRadius, 24.0f, 0, true);

	if (m_currentTargetTransform)
	{
		Vector3 targetPosition = TransformAPI::getPosition(m_currentTargetTransform);
		if (m_canSeeTarget)
		{
			DebugDrawAPI::drawLine(debugPosition, targetPosition, red, 0, true);
		}
		else
		{
			DebugDrawAPI::drawLine(debugPosition, targetPosition, yellow, 0, true);
		}
	}

	if (m_isAggro && !m_canSeeTarget)
	{
		DebugDrawAPI::drawCross(m_lastKnownTargetPosition, 0.35f, 0, true);
		DebugDrawAPI::drawPoint(m_lastKnownTargetPosition, cyan, 4.0f, 0, true);
	}
}

void ArthurDetectionAggro::enterAggro(Transform* target)
{
	if (!target)
	{
		return;
	}

	m_isAggro = true;
	m_canSeeTarget = true;
	m_currentTargetTransform = target;
	m_lastKnownTargetPosition = TransformAPI::getPosition(target);
}

void ArthurDetectionAggro::updateAggroState()
{
	updateAggroEntries();

	if (!m_encounterStarted)
	{
		m_currentTargetTransform = nullptr;
		m_isAggro = false;
		m_canSeeTarget = false;
		m_currentTargetLockTimer = 0.0f;
		m_tauntTargetTransform = nullptr;
		m_tauntTimer = 0.0f;
		return;
	}

	if (m_currentTargetTransform && isDeadTarget(m_currentTargetTransform))
	{
		m_currentTargetTransform = nullptr;
		m_isAggro = false;
		m_canSeeTarget = false;
		m_currentTargetLockTimer = 0.0f;
	}

	if (isTaunted())
	{
		if (!isTransformAlive(m_tauntTargetTransform))
		{
			clearTaunt(m_tauntTargetTransform);
		}
		else
		{
			m_isAggro = true;
			m_canSeeTarget = true;
			m_currentTargetTransform = m_tauntTargetTransform;
			m_lastKnownTargetPosition = TransformAPI::getPosition(m_tauntTargetTransform);
			return;
		}
	}

	if (m_phase == ArthurBossPhase::Phase1)
	{
		Transform* lyrielTarget = getLyrielTransform();

		if (isTransformAlive(lyrielTarget))
		{
			enterAggro(lyrielTarget);
			return;
		}
	}

	if (!m_isAggro)
	{
		Transform* initialTarget = selectClosestDetectedPlayer();

		if (initialTarget)
		{
			enterAggro(initialTarget);
			m_currentTargetLockTimer = 0.0f;
		}

		return;
	}

	const bool currentTargetStillDetected =
		isValidAliveTarget(m_currentTargetTransform) &&
		(
			(m_currentTargetTransform == m_LyrielAggro.targetTransform && m_LyrielAggro.isInDetectionRange) ||
			(m_currentTargetTransform == m_DeathAggro.targetTransform && m_DeathAggro.isInDetectionRange)
			);

	if (currentTargetStillDetected)
	{
		m_canSeeTarget = true;
		m_lastKnownTargetPosition = TransformAPI::getPosition(m_currentTargetTransform);
	}
	else
	{
		m_canSeeTarget = false;
	}

	if (!isTargetLockActive())
	{
		Transform* reevaluatedTarget = selectReevaluatedTarget();

		if (reevaluatedTarget && reevaluatedTarget != m_currentTargetTransform)
		{
			m_currentTargetTransform = reevaluatedTarget;
			m_lastKnownTargetPosition = TransformAPI::getPosition(m_currentTargetTransform);
		}

		const bool lyrielAggroing = isLyrielAggroing();
		const bool deathAggroing = isDeathAggroing();

		if (lyrielAggroing || deathAggroing)
		{
			startTargetLock();
		}
		else
		{
			m_currentTargetLockTimer = 0.0f;
		}
	}
}

void ArthurDetectionAggro::updateAggroEntries()
{
	Transform* player1 = getLyrielTransform();
	Transform* player2 = getDeathTransform();

	m_LyrielAggro.targetTransform = player1;
	m_DeathAggro.targetTransform = player2;

	m_LyrielAggro.isInDetectionRange = isLyrielInDetectionRange();
	m_DeathAggro.isInDetectionRange = isDeathInDetectionRange();

	m_LyrielAggro.distanceToEnemy = getDistanceToLyriel();
	m_DeathAggro.distanceToEnemy = getDistanceToDeath();
}

bool ArthurDetectionAggro::isTargetLockActive() const
{
	return m_currentTargetLockTimer > 0.0f;
}

void ArthurDetectionAggro::startTargetLock()
{
	m_currentTargetLockTimer = m_targetLockDuration;
}

void ArthurDetectionAggro::updateTargetLockTimer()
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

void ArthurDetectionAggro::updateTauntTimer()
{
	if (!isTaunted())
	{
		return;
	}

	m_tauntTimer -= Time::getDeltaTime();

	if (m_tauntTimer <= 0.0f)
	{
		clearTaunt(m_tauntTargetTransform);
	}
}

bool ArthurDetectionAggro::isTaunted() const
{
	return m_tauntTargetTransform != nullptr && m_tauntTimer > 0.0f;
}

Transform* ArthurDetectionAggro::selectClosestDetectedPlayer() const
{
	const bool lyrielInRange = m_LyrielAggro.isInDetectionRange && isValidAliveTarget(m_LyrielAggro.targetTransform);
	const bool deathInRange = m_DeathAggro.isInDetectionRange && isValidAliveTarget(m_DeathAggro.targetTransform);

	if (lyrielInRange && !deathInRange)
	{
		return m_LyrielAggro.targetTransform;
	}

	if (!lyrielInRange && deathInRange)
	{
		return m_DeathAggro.targetTransform;
	}

	if (lyrielInRange && deathInRange)
	{
		if (m_LyrielAggro.distanceToEnemy < m_DeathAggro.distanceToEnemy)
		{
			return m_LyrielAggro.targetTransform;
		}
		else
		{
			return m_DeathAggro.targetTransform;
		}
	}

	return nullptr;
}

Transform* ArthurDetectionAggro::selectReevaluatedTarget() const
{
	const bool lyrielAggroing = isLyrielAggroing() && isValidAliveTarget(m_LyrielAggro.targetTransform);
	const bool deathAggroing = isDeathAggroing() && isValidAliveTarget(m_DeathAggro.targetTransform);

	if (lyrielAggroing && !deathAggroing)
	{
		return m_LyrielAggro.targetTransform;
	}

	if (!lyrielAggroing && deathAggroing)
	{
		return m_DeathAggro.targetTransform;
	}

	if (lyrielAggroing && deathAggroing)
	{
		if (m_LyrielAggro.distanceToEnemy < m_DeathAggro.distanceToEnemy)
		{
			return m_LyrielAggro.targetTransform;
		}
		else
		{
			return m_DeathAggro.targetTransform;
		}
	}

	if (isValidAliveTarget(m_currentTargetTransform))
	{
		return m_currentTargetTransform;
	}

	return selectClosestDetectedPlayer();
}

void ArthurDetectionAggro::notifyPlayerAttackedEnemy(Transform* playerTransform)
{
	if (!isValidAliveTarget(playerTransform))
	{
		return;
	}

	AggroEntry* entry = getAggroEntry(playerTransform);

	if (!entry)
	{
		return;
	}

	entry->lastAttackTime = m_currentTime;

	if (!m_isAggro)
	{
		enterAggro(playerTransform);
		startTargetLock();
	}
}

void ArthurDetectionAggro::applyTaunt(Transform* playerTransform, float durationSeconds)
{
	if (playerTransform == nullptr || durationSeconds <= 0.0f)
	{
		return;
	}

	m_tauntTargetTransform = playerTransform;
	m_tauntTimer = durationSeconds;
	m_currentTargetLockTimer = 0.0f;
	enterAggro(playerTransform);

	// While taunted, range is ignored and line-of-sight is intentionally skipped.
	// TODO: Require wall checks / line-of-sight before applying the taunt effect.
	AggroEntry* entry = getAggroEntry(playerTransform);
	if (entry != nullptr)
	{
		entry->lastAttackTime = m_currentTime;
	}
}

void ArthurDetectionAggro::clearTaunt(Transform* playerTransform)
{
	if (playerTransform != nullptr && playerTransform != m_tauntTargetTransform)
	{
		return;
	}

	const bool wasCurrentTarget = (m_currentTargetTransform == m_tauntTargetTransform);

	m_tauntTargetTransform = nullptr;
	m_tauntTimer = 0.0f;

	if (!wasCurrentTarget)
	{
		return;
	}

	Transform* fallbackTarget = nullptr;

	if (m_phase == ArthurBossPhase::Phase1)
	{
		fallbackTarget = getLyrielTransform();
	}
	else
	{
		fallbackTarget = selectClosestDetectedPlayer();
	}

	if (fallbackTarget != nullptr)
	{
		m_currentTargetTransform = fallbackTarget;
		m_lastKnownTargetPosition = TransformAPI::getPosition(fallbackTarget);
		m_canSeeTarget = true;
		m_isAggro = true;
	}
	else
	{
		m_currentTargetTransform = nullptr;
		m_canSeeTarget = false;
		m_isAggro = false;
		m_currentTargetLockTimer = 0.0f;
	}
}

void ArthurDetectionAggro::setPhase(ArthurBossPhase phase)
{
	m_phase = phase;

	m_currentTargetLockTimer = 0.0f;

	if (phase == ArthurBossPhase::Phase2)
	{
		m_currentTargetTransform = selectClosestDetectedPlayer();
	}
}

void ArthurDetectionAggro::startEncounter()
{
	m_encounterStarted = true;

	m_phase = ArthurBossPhase::Phase1;

	m_isAggro = false;
	m_canSeeTarget = false;

	m_currentTargetTransform = nullptr;
	m_currentTargetLockTimer = 0.0f;

	m_tauntTargetTransform = nullptr;
	m_tauntTimer = 0.0f;
}

void ArthurDetectionAggro::stopEncounter()
{
	m_encounterStarted = false;

	m_isAggro = false;
	m_canSeeTarget = false;

	m_currentTargetTransform = nullptr;
	m_currentTargetLockTimer = 0.0f;

	m_tauntTargetTransform = nullptr;
	m_tauntTimer = 0.0f;
}

Transform* ArthurDetectionAggro::getOwnerTransform() const
{
	return GameObjectAPI::getTransform(getOwner());
}

Transform* ArthurDetectionAggro::getLyrielTransform() const
{
	return m_lyrielTransform.getReferencedComponent();
}

Transform* ArthurDetectionAggro::getDeathTransform() const
{
	return m_deathTransform.getReferencedComponent();
}

Vector3 ArthurDetectionAggro::getOwnerPosition() const
{
	Transform* ownerTransform = getOwnerTransform();
	if (!ownerTransform)
	{
		return Vector3(0.0f, 0.0f, 0.0f);
	}

	return TransformAPI::getPosition(ownerTransform);
}

Vector3 ArthurDetectionAggro::getLyrielPosition() const
{
	Transform* lyrielTransform = getLyrielTransform();
	if (!lyrielTransform)
	{
		return Vector3(0.0f, 0.0f, 0.0f);
	}

	return TransformAPI::getPosition(lyrielTransform);
}

Vector3 ArthurDetectionAggro::getDeathPosition() const
{
	Transform* deathTransform = getDeathTransform();
	if (!deathTransform)
	{
		return Vector3(0.0f, 0.0f, 0.0f);
	}

	return TransformAPI::getPosition(deathTransform);
}

float ArthurDetectionAggro::getDistanceToLyriel() const
{
	Vector3 difference = getLyrielPosition() - getOwnerPosition();
	return difference.Length();
}

float ArthurDetectionAggro::getDistanceToDeath() const
{
	Vector3 difference = getDeathPosition() - getOwnerPosition();
	return difference.Length();
}

bool ArthurDetectionAggro::isLyrielInDetectionRange() const
{
	if (!getLyrielTransform())
	{
		return false;
	}

	return getDistanceToLyriel() <= m_detectionRadius;
}

bool ArthurDetectionAggro::isDeathInDetectionRange() const
{
	if (!getDeathTransform())
	{
		return false;
	}

	return getDistanceToDeath() <= m_detectionRadius;
}

bool ArthurDetectionAggro::isLyrielAggroing() const
{
	if (!m_LyrielAggro.targetTransform)
	{
		return false;
	}

	return (m_currentTime - m_LyrielAggro.lastAttackTime) <= m_recentAttackMemory;
}

bool ArthurDetectionAggro::isDeathAggroing() const
{
	if (!m_DeathAggro.targetTransform)
	{
		return false;
	}

	return (m_currentTime - m_DeathAggro.lastAttackTime) <= m_recentAttackMemory;
}

bool ArthurDetectionAggro::isTransformAlive(Transform* target) const
{
	if (target == nullptr)
	{
		return false;
	}

	GameObject* targetOwner = ComponentAPI::getOwner(target);
	if (targetOwner == nullptr)
	{
		return false;
	}

	Damageable* damageable = GameObjectAPI::findScript<Damageable>(targetOwner);

	return damageable == nullptr || !damageable->isDead();
}

ArthurDetectionAggro::AggroEntry* ArthurDetectionAggro::getAggroEntry(Transform* target)
{
	if (target == getLyrielTransform())
	{
		return &m_LyrielAggro;
	}
	if (target == getDeathTransform())
	{
		return &m_DeathAggro;
	}
	return nullptr;
}

const ArthurDetectionAggro::AggroEntry* ArthurDetectionAggro::getAggroEntry(Transform* target) const
{
	if (target == getLyrielTransform())
	{
		return &m_LyrielAggro;
	}
	if (target == getDeathTransform())
	{
		return &m_DeathAggro;
	}
	return nullptr;
}

IMPLEMENT_SCRIPT(ArthurDetectionAggro)
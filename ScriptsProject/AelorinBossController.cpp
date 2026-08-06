#include "pch.h"
#include "AelorinBossController.h"

#include "AelorinDetectionAggro.h"
#include "AelorinDamageable.h"
#include "AelorinAttackConfig.h"
#include "AelorinAttackExecutor.h"

#include "HealthDropSpawner.h"
#include "ProjectilePool.h"
#include "SeekerSigilProjectile.h"

#include <algorithm>
#include <cstdlib>

IMPLEMENT_SCRIPT_FIELDS_INHERITED(AelorinBossController, EnemyBaseController,
	SERIALIZED_ASSET_REF(m_attackConfig, "Attack Config", AssetType::DATA_CONTAINER),
	SERIALIZED_COMPONENT_REF(m_seekerSigilsProjectilePool, "Seeker Sigils Projectile Pool", ComponentType::TRANSFORM),
	SERIALIZED_COMPONENT_REF(m_seekerSigilsLargeProjectilePool, "Seeker Sigils Large Projectile Pool", ComponentType::TRANSFORM)
)

AelorinBossController::AelorinBossController(GameObject* owner) : EnemyBaseController(owner)
{
}

void AelorinBossController::Start()
{
	EnemyBaseController::Start();

	m_aelorinDetectionAggro = GameObjectAPI::findScript<AelorinDetectionAggro>(getOwner());
	m_damageable = GameObjectAPI::findScript<AelorinDamageable>(getOwner());
	m_attackExecutor = GameObjectAPI::findScript<AelorinAttackExecutor>(getOwner());

	Transform* phase1Model = TransformAPI::findChildByName(getOwner()->GetTransform(), "Phase1");
	if (!phase1Model)
	{
		Debug::error("[AelorinBossController] Phase 1 Model not found!");
		return;
	}	

	Transform* phase2Model = TransformAPI::findChildByName(getOwner()->GetTransform(), "Phase2");
	if (!phase2Model)
	{
		Debug::error("[AelorinBossController] Phase 2 Model not found!");
		return;
	}

	m_phase1GameObject = ComponentAPI::getOwner(phase1Model);
	m_phase2GameObject = ComponentAPI::getOwner(phase2Model);

	if (!m_aelorinDetectionAggro)
	{
		Debug::error("[AelorinBossController] AelorinDetectionAggro script not found!");
	}

	if (!m_damageable)
	{
		Debug::error("[AelorinBossController] AelorinDamageable script not found!");
	}

	if (!m_attackExecutor)
	{
		Debug::error("[AelorinBossController] AelorinAttackExecutor script not found!");
	}

	if (!m_phase1GameObject)
	{
		Debug::error("[AelorinBossController] Phase 1 Game Object not found!");
	}

	if (!m_phase2GameObject)
	{
		Debug::error("[AelorinBossController] Phase 2 Game Object not found!");
	}

	Transform* normalPoolTransform = m_seekerSigilsProjectilePool.getReferencedComponent();
	if (normalPoolTransform)
	{
		m_seekerSigilsProjectilePoolScript = GameObjectAPI::findScript<ProjectilePool>(normalPoolTransform->getOwner());
	}

	Transform* largePoolTransform = m_seekerSigilsLargeProjectilePool.getReferencedComponent();
	if (largePoolTransform)
	{
		m_seekerSigilsLargeProjectilePoolScript = GameObjectAPI::findScript<ProjectilePool>(largePoolTransform->getOwner());
	}

	if (!m_seekerSigilsProjectilePoolScript)
	{
		Debug::error("[AelorinBossController] Normal Seeker Sigils ProjectilePool not found.");
	}

	if (!m_seekerSigilsLargeProjectilePoolScript)
	{
		Debug::error("[AelorinBossController] Large Seeker Sigils ProjectilePool not found.");
	}
}

//void AelorinBossController::drawGizmo()
//{
//
//}

void AelorinBossController::Update()
{
	updateEncounter();
}

Vector3 AelorinBossController::getLyrielPosition() const
{
	if (!m_aelorinDetectionAggro)
	{
		return Vector3::Zero;
	}

	return m_aelorinDetectionAggro->getLyrielPosition();
}

Vector3 AelorinBossController::getDeathPosition() const
{
	if (!m_aelorinDetectionAggro)
	{
		return Vector3::Zero;
	}

	return m_aelorinDetectionAggro->getDeathPosition();
}

float AelorinBossController::getClosestPlayerDistance() const
{
	if (!m_aelorinDetectionAggro)
	{
		return FLT_MAX;
	}

	const float minDistance = (std::min)(m_aelorinDetectionAggro->getDistanceToLyriel(), m_aelorinDetectionAggro->getDistanceToDeath());
	
	return minDistance;
}

AelorinAbility AelorinBossController::chooseNextAbility()
{
	if (canTeleport())
	{
		return AelorinAbility::Teleport;
	}

	std::vector<AelorinAbility> pool = buildAbilityPool();
	removeLastUsedAbility(pool);
	
	return chooseRandomAbility(pool);
}

AelorinAbility AelorinBossController::consumeRequestedAbility()
{
	const AelorinAbility ability = m_requestedAbility;

	if (ability != AelorinAbility::None)
	{
		m_lastUsedAbility = ability;
	}

	clearRequestedAbility();

	return ability;
}

bool AelorinBossController::requestAbility(AelorinAbility ability)
{
	if (ability == AelorinAbility::None || hasRequestedAbility())
	{
		return false;
	}

	m_requestedAbility = ability;
	m_abilityTriggerSent = false;

	return true;
}

bool AelorinBossController::trySendRequestedAbilityTrigger(AnimationComponent* animation)
{
	if (!hasRequestedAbility() || m_abilityTriggerSent || !animation)
	{
		return false;
	}

	const char* triggerName = nullptr;

	switch (m_requestedAbility)
	{
	case AelorinAbility::SeekerSigils:
		triggerName = "ToSeekerSigils";
		break;

	case AelorinAbility::Nova:
		triggerName = "ToNova";
		break;

	case AelorinAbility::RisenSpires:
		triggerName = "ToRisenSpires";
		break;

	case AelorinAbility::SpiritCannon:
		triggerName = "ToSpiritCannon";
		break;

	case AelorinAbility::GraspOfTheDead:
		triggerName = "ToGraspOfTheDead";
		break;

	case AelorinAbility::Summon:
		triggerName = "ToSummon";
		break;

	case AelorinAbility::Teleport:
		triggerName = "ToTeleport";
		break;

	case AelorinAbility::None:
	default:
		return false;
	}

	const bool sent = AnimationAPI::sendTrigger(animation, triggerName);

	if (!sent)
	{
		return false;
	}

	m_abilityTriggerSent = true;
	Debug::log("[AelorinBossController] Ability trigger sent: %s", triggerName);

	return true;
}

void AelorinBossController::clearRequestedAbility()
{
	m_requestedAbility = AelorinAbility::None;
	m_abilityTriggerSent = false;
}

void AelorinBossController::updateEncounter()
{
	if (!m_hasStartedEncounter && m_aelorinDetectionAggro)
	{
		if (m_aelorinDetectionAggro->startEncounter())
		{
			m_hasStartedEncounter = true;
		}
	}
}

float AelorinBossController::getDecisionTime() const
{
	const AelorinAttackConfig* config = m_attackConfig.get();

	if (!config)
	{
		return isPhase2() ? 1.0f : 2.0f;
	}

	return isPhase2() ? config->m_phase2DecisionTime : config->m_phase1DecisionTime;
}

void AelorinBossController::setPhase(Phase phase)
{
	m_phase = phase;
}

void AelorinBossController::requestPhaseTransition()
{
	if (isPhase2() || m_phaseTransitionRequested)
	{
		return;
	}

	m_phaseTransitionRequested = true;

	Debug::log("[AelorinBossController] Phase transition requested.");
}

void AelorinBossController::markPhaseTransitionTriggered()
{
	m_phaseTransitionTriggered = true;
}

void AelorinBossController::beginPhase2()
{
	if (isPhase2())
	{
		return;
	}

	if (!m_phase1GameObject || !m_phase2GameObject)
	{
		return;
	}

	GameObjectAPI::setActive(m_phase1GameObject, false);
	GameObjectAPI::setActive(m_phase2GameObject, true);

	setPhase(Phase::Phase2);

	m_phaseTransitionRequested = false;
	m_phaseTransitionTriggered = false;

	if (m_damageable)
	{
		m_damageable->beginPhase2();
	}

	Debug::log("[AelorinBossController] Phase 2 started.");
}

bool AelorinBossController::trySendPhaseTransitionTrigger(AnimationComponent* animation)
{
	if (!m_phaseTransitionRequested)
	{
		return false;
	}

	if (m_phaseTransitionTriggered)
	{
		return false;
	}

	if (!animation)
	{
		return false;
	}

	const bool sent = AnimationAPI::sendTrigger(animation, "ToPhaseTransition");

	if (!sent)
	{
		return false;
	}

	markPhaseTransitionTriggered();

	Debug::log("[AelorinBossController] ToPhaseTransition trigger sent.");

	return true;
}

void AelorinBossController::requestThresholdStagger()
{
	if (m_thresholdStaggerRequested)
	{
		return;
	}

	m_thresholdStaggerRequested = true;
	m_thresholdStaggerTriggered = false;

	Debug::log("[AelorinBossController] Threshold stagger requested.");
}

bool AelorinBossController::trySendThresholdStaggerTrigger(AnimationComponent* animation)
{
	if (!m_thresholdStaggerRequested)
	{
		return false;
	}

	if (m_thresholdStaggerTriggered)
	{
		return false;
	}

	if (!animation)
	{
		return false;
	}

	const bool sent = AnimationAPI::sendTrigger(animation, "ToThresholdStagger");

	if (!sent)
	{
		return false;
	}

	m_thresholdStaggerTriggered = true;

	Debug::log("[AelorinBossController] ToThresholdStagger trigger sent.");

	return true;
}

void AelorinBossController::completeThresholdStagger()
{
	m_thresholdStaggerRequested = false;
	m_thresholdStaggerTriggered = false;

	Debug::log("[AelorinBossController] Threshold stagger completed.");
}

float AelorinBossController::getThresholdStaggerDuration() const
{
	const AelorinAttackConfig* config = m_attackConfig.get();

	if (!config)
	{
		return 2.0f;
	}

	return config->m_thresholdStaggerDuration;
}

void AelorinBossController::spawnHealthDrops()
{
	const AelorinAttackConfig* config = m_attackConfig.get();

	if (!config)
	{
		Debug::warn("[AelorinBossController] Cannot spawn health drops: AelorinAttackConfig is missing.");
		return;
	}

	if (!config->m_healthPickupPrefab.m_id.isValid())
	{
		Debug::warn("[AelorinBossController] Cannot spawn health drops: health pickup prefab is invalid.");
		return;
	}

	Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
	if (!ownerTransform)
	{
		Debug::warn("[AelorinBossController] Cannot spawn health drops: owner transform is missing.");
		return;
	}

	const Vector3 position = TransformAPI::getGlobalPosition(ownerTransform);

	for (int i = 0; i < config->m_healthDropQuantity; ++i)
	{
		HealthDropSpawner::drop(
			config->m_healthPickupPrefab.m_id,
			position,
			config->m_healingAmount,
			config->m_dropRadius,
			config->m_dropHeight
		);
	}
}

// These two will not be needed
Transform* AelorinBossController::acquireCurrentTarget()
{
	return nullptr;
}

bool AelorinBossController::isTargetDowned(Transform* target) const
{
	return false;
}
// -----------------------------

std::vector<AelorinAbility> AelorinBossController::buildAbilityPool() const
{
	std::vector<AelorinAbility> pool
	{
		AelorinAbility::SeekerSigils
		//AelorinAbility::RisenSpires,
		//AelorinAbility::SpiritCannon
	};

	if (canUseNova())
	{
		pool.push_back(AelorinAbility::Nova);
	}

	//if (canSummon())
	//{
	//	pool.push_back(AelorinAbility::Summon);
	//}

	//if (isPhase2())
	//{
	//	pool.push_back(AelorinAbility::GraspOfTheDead);
	//}

	return pool;
}

void AelorinBossController::removeLastUsedAbility(std::vector<AelorinAbility>& pool) const
{
	if (m_lastUsedAbility == AelorinAbility::None || pool.size() <= 1)
	{
		return;
	}

	pool.erase(std::remove(pool.begin(), pool.end(), m_lastUsedAbility), pool.end());
}

bool AelorinBossController::canUseNova() const
{
	const AelorinAttackConfig* config = getAelorinAttackConfig();

	if (!config)
	{
		return false;
	}

	return isPlayerWithinDistance(config->m_novaTriggerDistance);
}

bool AelorinBossController::canSummon() const
{
	return false;
}

bool AelorinBossController::canTeleport() const
{
	return false;
}

AelorinAbility AelorinBossController::chooseRandomAbility(const std::vector<AelorinAbility>& pool) const
{
	if (pool.empty())
	{
		return AelorinAbility::None;
	}

	const std::size_t index = static_cast<std::size_t>(std::rand()) % pool.size();

	return pool[index];
}

bool AelorinBossController::isPlayerWithinDistance(float distance) const
{
	return getClosestPlayerDistance() <= distance;
}

IMPLEMENT_SCRIPT(AelorinBossController)
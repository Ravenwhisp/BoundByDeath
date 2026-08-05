#include "pch.h"
#include "AelorinSeekerSigilsState.h"
#include "AelorinAttackConfig.h"
#include "ProjectilePool.h"
#include "SeekerSigilProjectile.h"

AelorinSeekerSigilsState::AelorinSeekerSigilsState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void AelorinSeekerSigilsState::OnStateEnter()
{
	Transform* parentTransform = TransformAPI::getParent(getOwner()->GetTransform());
	GameObject* parentGameObject = ComponentAPI::getOwner(parentTransform);

	// get scripts
	m_controller = GameObjectAPI::findScript<AelorinBossController>(parentGameObject);
	m_animation = AnimationAPI::getAnimationComponent(getOwner());
	m_projectilePool = GameObjectAPI::findScript<ProjectilePool>(parentGameObject);

	// reset members
	m_activeAbility = AelorinAbility::None;
	m_waveTimer = 0.0f;
	m_currentWave = 0;
	m_finalProjectileLaunched = false;
	m_completed = false;

	if (!m_controller)
	{
		Debug::error("[AelorinSeekerSigilsState] AelorinBossController not found.");
	}

	if (!m_animation)
	{
		Debug::error("[AelorinSeekerSigilsState] AnimationComponent not found.");
	}

	if (!m_projectilePool)
	{
		Debug::error("[AelorinSeekerSigilsState] ProjectilePool not found.");
	}

	// consume ability
	m_activeAbility = m_controller->consumeRequestedAbility();
	if (m_activeAbility != AelorinAbility::SeekerSigils)
	{
		Debug::warn("[AelorinSeekerSigilsState] Unexpected requested ability!");
	}

	Debug::log("[AelorinSeekerSigilsState] ENTER");
}

void AelorinSeekerSigilsState::OnStateUpdate()
{
	if (!m_controller || !m_animation || !m_projectilePool || m_completed)
	{
		return;
	}

	const AelorinAttackConfig* config = m_controller->getAelorinAttackConfig();

	if (!config)
	{
		return;
	}

	m_waveTimer += Time::getDeltaTime();

	const float nextWaveTime = m_currentWave == 0 ? config->m_seekerSigilsInitialDelay : config->m_seekerSigilsWaveInterval;

	if (m_currentWave < config->m_seekerSigilsWaveCount)
	{
		if (m_waveTimer < nextWaveTime)
		{
			return;
		}

		launchCurrentWave();

		++m_currentWave;
		m_waveTimer = 0.0f;

		return;
	}

	if (m_controller->isPhase2() && !m_finalProjectileLaunched)
	{
		if (m_waveTimer < config->m_seekerSigilsPhase2FinalDelay)
		{
			return;
		}

		launchPhase2FinalProjectile();

		m_finalProjectileLaunched = true;
		m_waveTimer = 0.0f;
		
		return;
	}

	if (m_waveTimer < config->m_seekerSigilsRecoveryDuration)
	{
		return;
	}

	finishAbility();
}

void AelorinSeekerSigilsState::OnStateExit()
{
	m_waveTimer = 0.0f;
	m_currentWave = 0;
	m_finalProjectileLaunched = false;
	m_completed = false;

	Debug::log("[AelorinSeekerSigilsState] EXIT");
}

void AelorinSeekerSigilsState::launchCurrentWave()
{
	if (!m_controller)
	{
		return;
	}

	const AelorinAttackConfig* config = m_controller->getAelorinAttackConfig();

	if (!config)
	{
		return;
	}

	const Vector3 lyrielPosition = m_controller->getLyrielPosition();
	const Vector3 deathPosition = m_controller->getDeathPosition();

	launchProjectileAt(lyrielPosition, config->m_seekerSigilsRadius, config->m_seekerSigilsDamage);
	launchProjectileAt(deathPosition, config->m_seekerSigilsRadius, config->m_seekerSigilsDamage);

	Debug::log("[AelorinSeekerSigilsState] Launched wave %d.", m_currentWave + 1);
}

void AelorinSeekerSigilsState::launchProjectileAt(const Vector3& targetPosition, float impactRadius, float damage)
{
	if (!m_projectilePool || !m_controller)
	{
		return;
	}

	const AelorinAttackConfig* config = m_controller->getAelorinAttackConfig();

	if (!config)
	{
		return;
	}

	ProjectileBase* pooledProjectile = m_projectilePool->acquireProjectile();

	if (!pooledProjectile)
	{
		Debug::warn("[AelorinSeekerSigilsState] No available Seeker Sigil Projectile.");
		return;
	}

	SeekerSigilProjectile* projectile = static_cast<SeekerSigilProjectile*>(pooledProjectile);
	Vector3 impactPosition = targetPosition;
	impactPosition.y = targetPosition.y;

	Vector3 spawnPosition = impactPosition;
	spawnPosition.y += config->m_seekerSigilsSpawnHeight;

	projectile->launch(spawnPosition, impactPosition, config->m_seekerSigilsFallSpeed, config->m_seekerSigilsProjectileLifetime, impactRadius, damage);
}

void AelorinSeekerSigilsState::launchPhase2FinalProjectile()
{
	if (!m_controller)
	{
		return;
	}

	const AelorinAttackConfig* config = m_controller->getAelorinAttackConfig();

	if (!config)
	{
		return;
	}

	const Vector3 lyrielPosition = m_controller->getLyrielPosition();
	const Vector3 deathPosition = m_controller->getDeathPosition();
	const Vector3 midpoint = (lyrielPosition + deathPosition) * 0.5f;

	launchProjectileAt(midpoint, config->m_seekerSigilsPhase2FinalRadius, config->m_seekerSigilsPhase2FinalDamage);

	Debug::log("[AelorinSeekerSigilsState] Launched Phase 2 final projectile.");
}

void AelorinSeekerSigilsState::finishAbility()
{
	if (m_completed)
	{
		return;
	}

	m_completed = true;

	const bool sent = AnimationAPI::sendTrigger(m_animation, "ToIdle");
	if (!sent)
	{
		Debug::warn("[AelorinSeekerSigilsState] Failed to send ToIdle trigger.");
	}
}

IMPLEMENT_SCRIPT(AelorinSeekerSigilsState)
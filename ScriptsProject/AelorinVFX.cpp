#include "pch.h"
#include "AelorinVFX.h"

IMPLEMENT_SCRIPT_FIELDS(AelorinVFX,
	SERIALIZED_ASSET_REF(m_phase2AuraPrefab, "Phase 2 Aura Prefab", AssetType::PREFAB),
	SERIALIZED_ASSET_REF(m_phase2TransitionPrefab, "Phase 2 Transition Prefab", AssetType::PREFAB),
	SERIALIZED_ASSET_REF(m_phase1TeleportPrefab, "Phase 1 Teleport Prefab", AssetType::PREFAB),
	SERIALIZED_ASSET_REF(m_phase2TeleportPrefab, "Phase 2 Teleport Prefab", AssetType::PREFAB),
	SERIALIZED_ASSET_REF(m_summonEnemyPrefab, "Summon Enemy Prefab", AssetType::PREFAB)
)

AelorinVFX::AelorinVFX(GameObject* owner) : Script(owner)
{
}

void AelorinVFX::Update()
{
	m_oneShotVFX.update(Time::getDeltaTime());
}

void AelorinVFX::OnGameStop()
{
	m_oneShotVFX.clear();

	ParticleLifecycle::destroy(m_phase2AuraEffect);
	ParticleLifecycle::destroy(m_phase1TeleportEffect);
	ParticleLifecycle::destroy(m_phase2TeleportEffect);
}

void AelorinVFX::startPhase2Aura()
{
	if (m_phase2AuraActive)
	{
		return;
	}

	if (!m_phase2AuraPrefab.m_id.isValid())
	{
		Debug::warn("[AelorinVFX] Phase 2 Aura prefab is not assigned.");
		return;
	}

	Transform* ownerTransform =	GameObjectAPI::getTransform(getOwner());
	if (!ownerTransform)
	{
		return;
	}

	const Vector3 position = TransformAPI::getGlobalPosition(ownerTransform);
	const Vector3 rotation = TransformAPI::getGlobalEulerDegrees(ownerTransform);

	ParticleLifecycle::ensurePersistent(
		m_phase2AuraEffect,
		m_phase2AuraPrefab.m_id,
		position,
		rotation,
		getOwner()
	);

	if (!m_phase2AuraEffect)
	{
		Debug::warn("[AelorinVFX] Could not create Phase 2 Aura.");
		return;
	}

	ParticleLifecycle::activate(m_phase2AuraEffect);

	m_phase2AuraActive = true;
}

void AelorinVFX::stopPhase2Aura()
{
	m_phase2AuraActive = false;

	ParticleLifecycle::deactivate(m_phase2AuraEffect);
}

void AelorinVFX::playPhase2Transition()
{
	if (!m_phase2TransitionPrefab.m_id.isValid())
	{
		Debug::warn("[AelorinVFX] Phase 2 Transition prefab is not assigned.");
		return;
	}

	Transform* ownerTransform =	GameObjectAPI::getTransform(getOwner());
	if (!ownerTransform)
	{
		return;
	}

	const Vector3 position = TransformAPI::getGlobalPosition(ownerTransform);
	const Vector3 rotation = TransformAPI::getGlobalEulerDegrees(ownerTransform);

	ParticleLifecycle::spawnOneShotTimed(
		m_oneShotVFX,
		m_phase2TransitionPrefab.m_id,
		position,
		rotation,
		ParticleLifecycle::kDefaultOneShotLifetime,
		getOwner()
	);
}

void AelorinVFX::startPhase1Teleport()
{
	if (m_phase1TeleportActive)
	{
		return;
	}

	if (!m_phase1TeleportPrefab.m_id.isValid())
	{
		Debug::warn("[AelorinVFX] Phase 1 Teleport prefab is not assigned.");
		return;
	}

	Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
	if (!ownerTransform)
	{
		return;
	}
	
	const Vector3 position = TransformAPI::getGlobalPosition(ownerTransform);
	const Vector3 rotation = TransformAPI::getGlobalEulerDegrees(ownerTransform);

	ParticleLifecycle::ensurePersistent(
		m_phase1TeleportEffect,
		m_phase1TeleportPrefab.m_id,
		position,
		rotation,
		getOwner()
	);

	if (!m_phase1TeleportEffect)
	{
		Debug::warn("[AelorinVFX] Could not create Phase 1 Teleport effect.");
		return;
	}

	ParticleLifecycle::activate(m_phase1TeleportEffect);

	m_phase1TeleportActive = true;
}

void AelorinVFX::stopPhase1Teleport()
{
	m_phase1TeleportActive = false;

	ParticleLifecycle::deactivate(m_phase1TeleportEffect);
}

void AelorinVFX::startPhase2Teleport()
{
	if (m_phase2TeleportActive)
	{
		return;
	}

	if (!m_phase2TeleportPrefab.m_id.isValid())
	{
		Debug::warn("[AelorinVFX] Phase 2 Teleport prefab is not assigned.");
		return;
	}

	Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
	if (!ownerTransform)
	{
		return;
	}

	const Vector3 position = TransformAPI::getGlobalPosition(ownerTransform);
	const Vector3 rotation = TransformAPI::getGlobalEulerDegrees(ownerTransform);

	ParticleLifecycle::ensurePersistent(
		m_phase2TeleportEffect,
		m_phase2TeleportPrefab.m_id,
		position,
		rotation,
		getOwner()
	);

	if (!m_phase2TeleportEffect)
	{
		Debug::warn("[AelorinVFX] Could not create Phase 2 Teleport effect.");
		return;
	}

	ParticleLifecycle::activate(m_phase2TeleportEffect);

	m_phase2TeleportActive = true;
}

void AelorinVFX::stopPhase2Teleport()
{
	m_phase2TeleportActive = false;

	ParticleLifecycle::deactivate(m_phase2TeleportEffect);
}

void AelorinVFX::playSummonEnemyEffect(const Vector3& position)
{
	if (!m_summonEnemyPrefab.m_id.isValid())
	{
		Debug::warn("[AelorinVFX] Summon Enemy prefab is not assigned.");
		return;
	}

	ParticleLifecycle::spawnOneShotTimed(
		m_oneShotVFX,
		m_summonEnemyPrefab.m_id,
		position,
		Vector3::Zero,
		ParticleLifecycle::kDefaultOneShotLifetime,
		ParticleLifecycle::getRuntimeVfxContainer()
	);
}

IMPLEMENT_SCRIPT(AelorinVFX)
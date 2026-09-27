#include "pch.h"
#include "AelorinVFX.h"

IMPLEMENT_SCRIPT_FIELDS(AelorinVFX,
	SERIALIZED_ASSET_REF(m_phase2AuraPrefab, "Phase 2 Aura Prefab", AssetType::PREFAB)
)

AelorinVFX::AelorinVFX(GameObject* owner) : Script(owner)
{
}

void AelorinVFX::OnGameStop()
{
	ParticleLifecycle::destroy(m_phase2AuraEffect);
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

IMPLEMENT_SCRIPT(AelorinVFX)
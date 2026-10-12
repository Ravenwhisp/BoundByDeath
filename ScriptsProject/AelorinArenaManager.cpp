#include "pch.h"
#include "AelorinArenaManager.h"

IMPLEMENT_SCRIPT_FIELDS(AelorinArenaManager,
	FIELD_GROUP_COLLAPSE("Arena",
		SERIALIZED_COMPONENT_REF(m_phase1Origin, "Phase 1 Origin", ComponentType::TRANSFORM),
		SERIALIZED_COMPONENT_REF(m_phase2Origin, "Phase 2 Origin", ComponentType::TRANSFORM)
	),

	FIELD_GROUP_COLLAPSE("Encounter",
		SERIALIZED_COMPONENT_REF(m_aelorinRoot, "Aelorin Root", ComponentType::TRANSFORM),
		SERIALIZED_COMPONENT_REF(m_lyrielRoot, "Lyriel Root", ComponentType::TRANSFORM),
		SERIALIZED_COMPONENT_REF(m_deathRoot, "Death Root",	ComponentType::TRANSFORM)
	),

	FIELD_GROUP_COLLAPSE("Camera",
		SERIALIZED_COMPONENT_REF(m_cinematics, "Cinematics Root", ComponentType::TRANSFORM),
		SERIALIZED_COMPONENT_REF(m_arenaCamera, "Arena Camera Root", ComponentType::TRANSFORM),
		SERIALIZED_COMPONENT_REF(m_combatCamera, "Combat Camera Root", ComponentType::TRANSFORM)
	)
)

AelorinArenaManager::AelorinArenaManager(GameObject* owner)
	: Script(owner)
{
}

void AelorinArenaManager::Start()
{
	m_phase1OriginTransform = m_phase1Origin.getReferencedComponent();
	m_phase2OriginTransform = m_phase2Origin.getReferencedComponent();
	m_aelorinRootTransform = m_aelorinRoot.getReferencedComponent();
	m_lyrielRootTransform = m_lyrielRoot.getReferencedComponent();
	m_deathRootTransform = m_deathRoot.getReferencedComponent();
	m_cinematicsTransform = m_cinematics.getReferencedComponent();
	m_arenaCameraTransform = m_arenaCamera.getReferencedComponent();
	m_combatCameraTransform = m_combatCamera.getReferencedComponent();
}

void AelorinArenaManager::switchToPhase2()
{
	if (m_phase2Active)
	{
		return;
	}

	if (!m_phase1OriginTransform ||	!m_phase2OriginTransform)
	{
		Debug::warn("[AelorinArenaManager] Arena origins are missing.");
		return;
	}

	moveToPhase2(m_aelorinRootTransform);
	moveToPhase2(m_lyrielRootTransform);
	moveToPhase2(m_deathRootTransform);
	moveToPhase2(m_cinematicsTransform);
	moveToPhase2(m_arenaCameraTransform);
	moveToPhase2(m_combatCameraTransform);

	m_phase2Active = true;

	Debug::log("[AelorinArenaManager] Switched encounter to Phase 2 arena.");
}

void AelorinArenaManager::moveToPhase2(Transform* target)
{
	if (!target)
	{
		return;
	}

	const Vector3 phase1Origin = TransformAPI::getGlobalPosition(m_phase1OriginTransform);
	const Vector3 phase2Origin = TransformAPI::getGlobalPosition(m_phase2OriginTransform);

	const Vector3 currentPosition = TransformAPI::getGlobalPosition(target);
	const Vector3 arenaOffset =	phase2Origin - phase1Origin;

	TransformAPI::setGlobalPosition(target,	currentPosition + arenaOffset);
}

IMPLEMENT_SCRIPT(AelorinArenaManager)
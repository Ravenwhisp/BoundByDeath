#include "pch.h"
#include "CheckpointSetup.h"

#include "PersistingCheckpointState.h"

IMPLEMENT_SCRIPT_FIELDS(CheckpointSetup,
	SERIALIZED_COMPONENT_REF(m_lyrielTransform, "Lyriel Transform", ComponentType::TRANSFORM),
	SERIALIZED_COMPONENT_REF(m_deathTransform, "Death Transform", ComponentType::TRANSFORM)
)

CheckpointSetup::CheckpointSetup(GameObject* owner)
    : Script(owner)
{
}

void CheckpointSetup::Start()
{
	m_PersistingCheckpointState = &PersistingCheckpointState::Get();

	if(!m_PersistingCheckpointState)
	{
		Debug::warn("CheckpointSetup: PersistingCheckpointState singleton not found.");
		return;
	}

	//Recorrer los hijos de este gameobject. En teoria deberian haber dos (por como esta hecho el prefab).
	Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());

	Transform* lyrielSpawnTransform = TransformAPI::findChildByName(ownerTransform, "LyrielSpawn");
	if(lyrielSpawnTransform)
	{
		lyrielSpawnPosition = TransformAPI::getGlobalPosition(lyrielSpawnTransform);
	}
	else
	{
		Debug::error("CheckpointSetup: %s GameObject has no children named \"LyrielSpawn\".", getOwner()->GetName());
	}


	Transform* deathSpawnTransform = TransformAPI::findChildByName(ownerTransform, "DeathSpawn");
	if (deathSpawnTransform)
	{
		deathSpawnPosition = TransformAPI::getGlobalPosition(deathSpawnTransform);
	}
	else
	{
		Debug::error("CheckpointSetup: %s GameObject has no children named \"DeathSpawn\".", getOwner()->GetName());
	}
}

void CheckpointSetup::ApplyCheckpointState()
{
	Transform* lyrielTransform = m_lyrielTransform.getReferencedComponent();
	if (lyrielTransform)
	{
		TransformAPI::setGlobalPosition(lyrielTransform, lyrielSpawnPosition);
	}
	else
	{
		Debug::warn("CheckpointSetup: %s GameObject has no Lyriel Transform referenced.", getOwner()->GetName());
	}

	Transform* deathTransform = m_deathTransform.getReferencedComponent();
	if (deathTransform)
	{
		TransformAPI::setGlobalPosition(deathTransform, deathSpawnPosition);
	}
	else
	{
		Debug::warn("CheckpointSetup: %s GameObject has no Death Transform referenced.", getOwner()->GetName());
	}
}

IMPLEMENT_SCRIPT(CheckpointSetup)
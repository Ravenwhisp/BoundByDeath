#include "pch.h"
#include "PersistingCheckpointState.h"

#include "CheckpointSetup.h"
#include "EnemyDamageable.h"
#include "BreakableObject.h"
#include "PowerupCollectible.h"

PersistingCheckpointState& PersistingCheckpointState::Get() 
{
	static PersistingCheckpointState instance;
	return instance;
}

void PersistingCheckpointState::SetCheckpoint(CheckpointId checkpointId)
{
	m_lastCheckpointId = checkpointId;

	m_deadEnemiesPersistent.insert(
		m_deadEnemiesPersistent.end(), 
		m_deadEnemies.begin(), 
		m_deadEnemies.end());
	m_brokenBreakablesPersistent.insert(
		m_brokenBreakablesPersistent.end(), 
		m_brokenBreakables.begin(), 
		m_brokenBreakables.end());
	m_collectedCollectiblesPersistent.insert(
		m_collectedCollectiblesPersistent.end(),
		m_collectedCollectibles.begin(),
		m_collectedCollectibles.end());

	m_solvedPuzzlesPersistent = m_solvedPuzzles;
}
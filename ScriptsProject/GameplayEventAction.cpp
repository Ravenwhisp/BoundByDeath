#include "pch.h"
#include "GameplayEventAction.h"

#include "PersistingCheckpointState.h"
#include "GameplayEventTrigger.h"

void GameplayEventAction::saveTriggeredEvent(GameplayEventTrigger* trigger)
{
	if(m_isPersistent && trigger->m_triggerOnlyOnce)
	{ 
		PersistingCheckpointState::Get().m_triggeredEvents.push_back(m_owner->GetID());
	}
}

#pragma once

#include "GameplayEventAction.h"
#include "BarkManager.h"

#include <string>
#include <vector>

class GameplayEventTrigger;

class BarkEvent : public GameplayEventAction
{
	DECLARE_SCRIPT(BarkEvent)

public:
	explicit BarkEvent(GameObject* owner);

	void executeEvent(GameplayEventTrigger* trigger) override;

	FieldList getExposedFields() const override;

private:
	BarkManager* findBarkManager() const;

private:
	std::vector<std::string> m_barks;

	float m_defaultDuration = 3.0f;
};
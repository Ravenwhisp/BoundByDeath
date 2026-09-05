#include "pch.h"
#include "BarkEvent.h"

#include "GameplayEventTrigger.h"

#include <algorithm>

static const char* barkSpeakerNames[] =
{
	"Death",
	"Lyriel"
};

constexpr int barkSpeakerCount = 2;

IMPLEMENT_SCRIPT_FIELDS(BarkEvent,
	SERIALIZED_ENUM_INT_VECTOR(
		m_speakers,
		"Speakers",
		barkSpeakerNames,
		barkSpeakerCount
	),

	SERIALIZED_STRING_VECTOR(
		m_texts,
		"Texts"
	),

	SERIALIZED_FLOAT_VECTOR(
		m_durations,
		"Durations"
	),

	SERIALIZED_FLOAT(
		m_defaultDuration,
		"Default Duration",
		0.1f,
		10.0f,
		0.1f
	)
)

BarkEvent::BarkEvent(GameObject* owner)
	: GameplayEventAction(owner)
{
	m_isPersistent = true;
}

void BarkEvent::executeEvent(GameplayEventTrigger* trigger)
{
	if (m_texts.empty())
	{
		Debug::warn(
			"BarkEvent on '%s' has no bark texts.",
			GameObjectAPI::getName(getOwner())
		);
		return;
	}

	if (m_speakers.size() != m_texts.size())
	{
		Debug::warn(
			"BarkEvent on '%s' requires the same number of Speakers and Texts.",
			GameObjectAPI::getName(getOwner())
		);
		return;
	}

	BarkManager* barkManager = findBarkManager();

	if (barkManager == nullptr)
	{
		Debug::warn(
			"BarkEvent on '%s' could not find BarkManager in the scene.",
			GameObjectAPI::getName(getOwner())
		);
		return;
	}

	std::vector<BarkLine> barks;
	barks.reserve(m_texts.size());

	for (size_t i = 0; i < m_texts.size(); ++i)
	{
		if (m_texts[i].empty())
		{
			continue;
		}

		const int speakerIndex = m_speakers[i];

		if (speakerIndex < 0 || speakerIndex >= barkSpeakerCount)
		{
			Debug::warn(
				"BarkEvent on '%s' has an invalid speaker at index %d.",
				GameObjectAPI::getName(getOwner()),
				static_cast<int>(i)
			);
			continue;
		}

		float duration = m_defaultDuration;

		if (i < m_durations.size() && m_durations[i] > 0.0f)
		{
			duration = m_durations[i];
		}

		BarkLine bark;
		bark.speaker = static_cast<BarkSpeaker>(speakerIndex);
		bark.text = m_texts[i];
		bark.duration = duration;

		barks.push_back(bark);
	}

	if (barks.empty())
	{
		Debug::warn(
			"BarkEvent on '%s' has no valid bark lines.",
			GameObjectAPI::getName(getOwner())
		);
		return;
	}

	barkManager->playBarks(barks);
}

BarkManager* BarkEvent::findBarkManager() const
{
	const std::vector<GameObject*> barkManagerObjects =
		SceneAPI::findAllGameObjectsWithScript<BarkManager>();

	if (barkManagerObjects.empty())
	{
		return nullptr;
	}

	return GameObjectAPI::findScript<BarkManager>(
		barkManagerObjects[0]
	);
}

IMPLEMENT_SCRIPT(BarkEvent)
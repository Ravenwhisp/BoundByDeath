#include "pch.h"
#include "BarkEvent.h"

#include "GameplayEventTrigger.h"

#include <string>
#include <vector>

IMPLEMENT_SCRIPT_FIELDS(BarkEvent,
	SERIALIZED_STRING_VECTOR(
		m_barks,
		"Barks (Speaker|Duration|Text)"
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
	if (m_barks.empty())
	{
		Debug::warn(
			"BarkEvent on '%s' has no barks.",
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
	barks.reserve(m_barks.size());

	for (size_t i = 0; i < m_barks.size(); ++i)
	{
		const std::string& barkData = m_barks[i];

		if (barkData.empty())
		{
			continue;
		}

		const size_t firstSeparator = barkData.find('|');

		if (firstSeparator == std::string::npos)
		{
			Debug::warn(
				"BarkEvent on '%s' has an invalid bark at index %d. Expected Speaker|Duration|Text.",
				GameObjectAPI::getName(getOwner()),
				static_cast<int>(i)
			);
			continue;
		}

		const size_t secondSeparator = barkData.find('|', firstSeparator + 1);

		if (secondSeparator == std::string::npos)
		{
			Debug::warn(
				"BarkEvent on '%s' has an invalid bark at index %d. Expected Speaker|Duration|Text.",
				GameObjectAPI::getName(getOwner()),
				static_cast<int>(i)
			);
			continue;
		}

		const std::string speakerText =
			barkData.substr(0, firstSeparator);

		const std::string durationText =
			barkData.substr(
				firstSeparator + 1,
				secondSeparator - firstSeparator - 1
			);

		const std::string text =
			barkData.substr(secondSeparator + 1);

		if (text.empty())
		{
			continue;
		}

		BarkSpeaker speaker;

		if (speakerText == "Death")
		{
			speaker = BarkSpeaker::Death;
		}
		else if (speakerText == "Lyriel")
		{
			speaker = BarkSpeaker::Lyriel;
		}
		else
		{
			Debug::warn(
				"BarkEvent on '%s' has an invalid speaker '%s' at index %d.",
				GameObjectAPI::getName(getOwner()),
				speakerText.c_str(),
				static_cast<int>(i)
			);
			continue;
		}

		float duration = m_defaultDuration;

		if (!durationText.empty())
		{
			try
			{
				const float parsedDuration = std::stof(durationText);

				if (parsedDuration > 0.0f)
				{
					duration = parsedDuration;
				}
			}
			catch (...)
			{
				Debug::warn(
					"BarkEvent on '%s' has an invalid duration '%s' at index %d. Using default duration.",
					GameObjectAPI::getName(getOwner()),
					durationText.c_str(),
					static_cast<int>(i)
				);
			}
		}

		BarkLine bark;
		bark.speaker = speaker;
		bark.text = text;
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
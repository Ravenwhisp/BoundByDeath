#include "pch.h"
#include "BarkEvent.h"

#include <random>
#include <string>
#include <vector>

namespace
{
	const char* barkPlayModeNames[] =
	{
		"Sequence",
		"Random One",
		"Random Sequence"
	};

	constexpr int barkPlayModeCount = 3;

	const char* barkPriorityNames[] =
	{
		"Normal",
		"High"
	};

	constexpr int barkPriorityCount = 2;

	std::mt19937& getBarkRandomGenerator()
	{
		static std::random_device randomDevice;

		static std::mt19937 generator(
			randomDevice()
		);

		return generator;
	}
}

IMPLEMENT_SCRIPT_FIELDS(
	BarkEvent,

	SERIALIZED_STRING_VECTOR(
		m_barks,
		"Barks:\nSequence/Random One: Speaker|Duration|Text\nRandom Sequence: Group|Speaker|Duration|Text"
	),

	SERIALIZED_ENUM_INT(
		m_playMode,
		"Play Mode",
		barkPlayModeNames,
		barkPlayModeCount
	),

	SERIALIZED_ENUM_INT(
		m_priority,
		"Priority",
		barkPriorityNames,
		barkPriorityCount
	),

	SERIALIZED_FLOAT(
		m_defaultDuration,
		"Default Duration",
		0.1f,
		10.0f,
		0.1f
	),

	SERIALIZED_FLOAT(
		m_cooldown,
		"Cooldown",
		0.0f,
		60.0f,
		0.1f
	),

	SERIALIZED_FLOAT(
		m_triggerChance,
		"Trigger Chance",
		0.0f,
		1.0f,
		0.05f
	)
)

BarkEvent::BarkEvent(GameObject* owner)
	: GameplayEventAction(owner)
{
	m_isPersistent = true;
}

void BarkEvent::Update()
{
	if (m_cooldownRemaining <= 0.0f)
	{
		return;
	}

	m_cooldownRemaining -=
		Time::getDeltaTime();

	if (m_cooldownRemaining < 0.0f)
	{
		m_cooldownRemaining = 0.0f;
	}
}

void BarkEvent::executeEvent(
	GameplayEventTrigger* trigger
)
{
	play();
}

bool BarkEvent::play()
{
	if (m_barks.empty())
	{
		Debug::warn(
			"BarkEvent on '%s' has no barks.",
			GameObjectAPI::getName(getOwner())
		);

		return false;
	}

	if (m_cooldownRemaining > 0.0f)
	{
		return false;
	}

	if (!passesTriggerChance())
	{
		return false;
	}

	BarkManager* barkManager =
		findBarkManager();

	if (barkManager == nullptr)
	{
		Debug::warn(
			"BarkEvent on '%s' could not find BarkManager in the scene.",
			GameObjectAPI::getName(getOwner())
		);

		return false;
	}

	const BarkPlayMode playMode =
		static_cast<BarkPlayMode>(
			m_playMode
			);

	std::vector<BarkLine> selectedBarks;

	int selectedRandomIndex = -1;

	switch (playMode)
	{
	case BarkPlayMode::Sequence:
	{
		selectedBarks =
			buildValidBarks();

		break;
	}

	case BarkPlayMode::RandomOne:
	{
		std::vector<BarkLine> validBarks =
			buildValidBarks();

		if (validBarks.empty())
		{
			Debug::warn(
				"BarkEvent on '%s' has no valid barks.",
				GameObjectAPI::getName(getOwner())
			);

			return false;
		}

		selectedRandomIndex =
			selectRandomIndex(
				static_cast<int>(
					validBarks.size()
					),
				m_lastRandomIndex
			);

		if (selectedRandomIndex < 0)
		{
			return false;
		}

		selectedBarks.push_back(
			validBarks[
				selectedRandomIndex
			]
		);

		break;
	}

	case BarkPlayMode::RandomSequence:
	{
		std::vector<std::vector<BarkLine>>
			sequences =
			buildValidRandomSequences();

		if (sequences.empty())
		{
			Debug::warn(
				"BarkEvent on '%s' has no valid random sequences.",
				GameObjectAPI::getName(getOwner())
			);

			return false;
		}

		selectedRandomIndex =
			selectRandomIndex(
				static_cast<int>(
					sequences.size()
					),
				m_lastRandomSequenceIndex
			);

		if (selectedRandomIndex < 0)
		{
			return false;
		}

		selectedBarks =
			sequences[
				selectedRandomIndex
			];

		break;
	}

	default:
		return false;
	}

	if (selectedBarks.empty())
	{
		Debug::warn(
			"BarkEvent on '%s' has no valid barks.",
			GameObjectAPI::getName(getOwner())
		);

		return false;
	}

	const bool accepted =
		barkManager->playBarks(
			selectedBarks
		);

	if (!accepted)
	{
		return false;
	}

	switch (playMode)
	{
	case BarkPlayMode::RandomOne:
		m_lastRandomIndex =
			selectedRandomIndex;
		break;

	case BarkPlayMode::RandomSequence:
		m_lastRandomSequenceIndex =
			selectedRandomIndex;
		break;

	default:
		break;
	}

	m_cooldownRemaining =
		m_cooldown;

	return true;
}

std::vector<BarkLine>
BarkEvent::buildValidBarks() const
{
	std::vector<BarkLine> validBarks;

	validBarks.reserve(
		m_barks.size()
	);

	for (
		size_t i = 0;
		i < m_barks.size();
		++i
		)
	{
		const std::string& barkData =
			m_barks[i];

		if (barkData.empty())
		{
			continue;
		}

		const size_t firstSeparator =
			barkData.find('|');

		if (
			firstSeparator ==
			std::string::npos
			)
		{
			Debug::warn(
				"BarkEvent on '%s' has an invalid bark at index %d. Expected Speaker|Duration|Text.",
				GameObjectAPI::getName(
					getOwner()
				),
				static_cast<int>(i)
			);

			continue;
		}

		const size_t secondSeparator =
			barkData.find(
				'|',
				firstSeparator + 1
			);

		if (
			secondSeparator ==
			std::string::npos
			)
		{
			Debug::warn(
				"BarkEvent on '%s' has an invalid bark at index %d. Expected Speaker|Duration|Text.",
				GameObjectAPI::getName(
					getOwner()
				),
				static_cast<int>(i)
			);

			continue;
		}

		const std::string speakerText =
			barkData.substr(
				0,
				firstSeparator
			);

		const std::string durationText =
			barkData.substr(
				firstSeparator + 1,
				secondSeparator
				- firstSeparator
				- 1
			);

		const std::string text =
			barkData.substr(
				secondSeparator + 1
			);

		bool valid = false;

		BarkLine bark =
			buildBarkLine(
				speakerText,
				durationText,
				text,
				static_cast<int>(i),
				valid
			);

		if (valid)
		{
			validBarks.push_back(
				bark
			);
		}
	}

	return validBarks;
}

std::vector<std::vector<BarkLine>>
BarkEvent::buildValidRandomSequences() const
{
	std::vector<int> groupIds;

	std::vector<std::vector<BarkLine>>
		sequences;

	for (
		size_t i = 0;
		i < m_barks.size();
		++i
		)
	{
		const std::string& barkData =
			m_barks[i];

		if (barkData.empty())
		{
			continue;
		}

		const size_t firstSeparator =
			barkData.find('|');

		if (
			firstSeparator ==
			std::string::npos
			)
		{
			Debug::warn(
				"BarkEvent on '%s' has an invalid Random Sequence bark at index %d. Expected Group|Speaker|Duration|Text.",
				GameObjectAPI::getName(
					getOwner()
				),
				static_cast<int>(i)
			);

			continue;
		}

		const size_t secondSeparator =
			barkData.find(
				'|',
				firstSeparator + 1
			);

		if (
			secondSeparator ==
			std::string::npos
			)
		{
			Debug::warn(
				"BarkEvent on '%s' has an invalid Random Sequence bark at index %d. Expected Group|Speaker|Duration|Text.",
				GameObjectAPI::getName(
					getOwner()
				),
				static_cast<int>(i)
			);

			continue;
		}

		const size_t thirdSeparator =
			barkData.find(
				'|',
				secondSeparator + 1
			);

		if (
			thirdSeparator ==
			std::string::npos
			)
		{
			Debug::warn(
				"BarkEvent on '%s' has an invalid Random Sequence bark at index %d. Expected Group|Speaker|Duration|Text.",
				GameObjectAPI::getName(
					getOwner()
				),
				static_cast<int>(i)
			);

			continue;
		}

		const std::string groupText =
			barkData.substr(
				0,
				firstSeparator
			);

		const std::string speakerText =
			barkData.substr(
				firstSeparator + 1,
				secondSeparator
				- firstSeparator
				- 1
			);

		const std::string durationText =
			barkData.substr(
				secondSeparator + 1,
				thirdSeparator
				- secondSeparator
				- 1
			);

		const std::string text =
			barkData.substr(
				thirdSeparator + 1
			);

		int groupId = 0;

		try
		{
			groupId =
				std::stoi(
					groupText
				);
		}
		catch (...)
		{
			Debug::warn(
				"BarkEvent on '%s' has invalid group '%s' at index %d.",
				GameObjectAPI::getName(
					getOwner()
				),
				groupText.c_str(),
				static_cast<int>(i)
			);

			continue;
		}

		bool valid = false;

		BarkLine bark =
			buildBarkLine(
				speakerText,
				durationText,
				text,
				static_cast<int>(i),
				valid
			);

		if (!valid)
		{
			continue;
		}

		int sequenceIndex = -1;

		for (
			size_t groupIndex = 0;
			groupIndex < groupIds.size();
			++groupIndex
			)
		{
			if (
				groupIds[groupIndex] ==
				groupId
				)
			{
				sequenceIndex =
					static_cast<int>(
						groupIndex
						);

				break;
			}
		}

		if (sequenceIndex < 0)
		{
			groupIds.push_back(
				groupId
			);

			sequences.push_back(
				std::vector<BarkLine>()
			);

			sequenceIndex =
				static_cast<int>(
					sequences.size() - 1
					);
		}

		sequences[
			sequenceIndex
		].push_back(
			bark
		);
	}

	return sequences;
}

BarkLine BarkEvent::buildBarkLine(
	const std::string& speakerText,
	const std::string& durationText,
	const std::string& text,
	int sourceIndex,
	bool& valid
) const
{
	valid = false;

	BarkLine bark;

	if (text.empty())
	{
		return bark;
	}

	if (speakerText == "Death")
	{
		bark.speaker =
			BarkSpeaker::Death;
	}
	else if (
		speakerText == "Lyriel"
		)
	{
		bark.speaker =
			BarkSpeaker::Lyriel;
	}
	else
	{
		Debug::warn(
			"BarkEvent on '%s' has invalid speaker '%s' at index %d.",
			GameObjectAPI::getName(
				getOwner()
			),
			speakerText.c_str(),
			sourceIndex
		);

		return bark;
	}

	float duration =
		m_defaultDuration;

	if (!durationText.empty())
	{
		try
		{
			const float parsedDuration =
				std::stof(
					durationText
				);

			if (parsedDuration > 0.0f)
			{
				duration =
					parsedDuration;
			}
		}
		catch (...)
		{
			Debug::warn(
				"BarkEvent on '%s' has invalid duration '%s' at index %d. Using default duration.",
				GameObjectAPI::getName(
					getOwner()
				),
				durationText.c_str(),
				sourceIndex
			);
		}
	}

	bark.text = text;
	bark.duration = duration;

	bark.priority =
		static_cast<BarkPriority>(
			m_priority
			);

	valid = true;

	return bark;
}

int BarkEvent::selectRandomIndex(
	int count,
	int lastIndex
) const
{
	if (count <= 0)
	{
		return -1;
	}

	if (count == 1)
	{
		return 0;
	}

	if (
		lastIndex < 0 ||
		lastIndex >= count
		)
	{
		std::uniform_int_distribution<int>
			distribution(
				0,
				count - 1
			);

		return distribution(
			getBarkRandomGenerator()
		);
	}

	std::uniform_int_distribution<int>
		distribution(
			0,
			count - 2
		);

	int randomIndex =
		distribution(
			getBarkRandomGenerator()
		);

	if (randomIndex >= lastIndex)
	{
		++randomIndex;
	}

	return randomIndex;
}

bool BarkEvent::passesTriggerChance()
{
	if (m_triggerChance >= 1.0f)
	{
		return true;
	}

	if (m_triggerChance <= 0.0f)
	{
		return false;
	}

	std::uniform_real_distribution<float>
		distribution(
			0.0f,
			1.0f
		);

	return distribution(
		getBarkRandomGenerator()
	) <= m_triggerChance;
}

BarkManager*
BarkEvent::findBarkManager() const
{
	const std::vector<GameObject*>
		barkManagerObjects =
		SceneAPI::
		findAllGameObjectsWithScript<
		BarkManager
		>();

	if (barkManagerObjects.empty())
	{
		return nullptr;
	}

	return GameObjectAPI::
		findScript<BarkManager>(
			barkManagerObjects[0]
		);
}

IMPLEMENT_SCRIPT(BarkEvent)
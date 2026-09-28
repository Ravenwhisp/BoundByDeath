#include "pch.h"
#include "BarkManager.h"

#include <algorithm>

BarkManager::BarkManager(GameObject* owner)
	: Script(owner)
{
}

void BarkManager::Start()
{
	m_barkText =
		UITextAPI::getTextComponent(getOwner());

	if (m_barkText == nullptr)
	{
		Debug::warn(
			"BarkManager on '%s' could not find a UIText component.",
			GameObjectAPI::getName(getOwner())
		);

		return;
	}

	Component* transformComponent =
		GameObjectAPI::getComponent(
			getOwner(),
			ComponentType::TRANSFORM2D
		);

	if (transformComponent != nullptr)
	{
		m_barkTransform =
			reinterpret_cast<Transform2D*>(
				transformComponent
				);

		m_barkBasePosition =
			Transform2DAPI::getPosition(
				m_barkTransform
			);
	}
	else
	{
		Debug::warn(
			"BarkManager on '%s' could not find a Transform2D component.",
			GameObjectAPI::getName(getOwner())
		);
	}

	clearBarkText();
}

void BarkManager::Update()
{
	if (!m_hasCurrentBark)
	{
		startNextBark();

		if (!m_hasCurrentBark)
		{
			return;
		}
	}

	m_timer -= Time::getDeltaTime();

	if (m_timer <= 0.0f)
	{
		finishCurrentBark();
	}
}

bool BarkManager::playBarks(
	const std::vector<BarkLine>& barks
)
{
	if (barks.empty())
	{
		return false;
	}

	const BarkPriority incomingPriority =
		barks.front().priority;

	if (m_hasCurrentBark)
	{
		const BarkPriority currentPriority =
			m_currentBark.priority;

		// Higher priority interrupts the current bark.
		if (isHigherPriority(
			incomingPriority,
			currentPriority
		))
		{
			interruptCurrentBark();

			// Remove pending lower-priority reactions.
			removeQueuedBarksBelow(
				incomingPriority
			);

			// The high-priority bark must play next.
			pushBarksToFront(barks);

			startNextBark();

			return true;
		}

		// Do not play a lower-priority bark while
		// a higher-priority one is active.
		if (isHigherPriority(
			currentPriority,
			incomingPriority
		))
		{
			return false;
		}
	}

	for (const BarkLine& bark : barks)
	{
		if (bark.text.empty())
		{
			continue;
		}

		m_barkQueue.push_back(bark);
	}

	if (!m_hasCurrentBark)
	{
		startNextBark();
	}

	return true;
}

void BarkManager::startNextBark()
{
	if (m_barkQueue.empty())
	{
		m_hasCurrentBark = false;
		m_timer = 0.0f;

		clearBarkText();
		return;
	}

	m_currentBark =
		m_barkQueue.front();

	m_barkQueue.pop_front();

	m_timer =
		m_currentBark.duration;

	m_hasCurrentBark = true;

	showCurrentBark();

	Debug::log(
		"Bark started - %s: %s",
		getSpeakerName(
			m_currentBark.speaker
		),
		m_currentBark.text.c_str()
	);
}

void BarkManager::finishCurrentBark()
{
	m_hasCurrentBark = false;
	m_timer = 0.0f;

	startNextBark();
}

void BarkManager::showCurrentBark()
{
	if (m_barkText == nullptr)
	{
		return;
	}

	const std::string displayText =
		std::string(
			getSpeakerName(
				m_currentBark.speaker
			)
		)
		+ ": "
		+ m_currentBark.text;

	UITextAPI::setText(
		m_barkText,
		displayText.c_str()
	);

	centerBarkText(displayText);
}

void BarkManager::clearBarkText()
{
	if (m_barkText != nullptr)
	{
		UITextAPI::setText(
			m_barkText,
			""
		);
	}

	if (m_barkTransform != nullptr)
	{
		Transform2DAPI::setPosition(
			m_barkTransform,
			m_barkBasePosition
		);
	}
}

void BarkManager::centerBarkText(
	const std::string& text
)
{
	if (m_barkTransform == nullptr)
	{
		return;
	}

	const float textWidth =
		estimateTextWidth(text);

	Vector2 position =
		m_barkBasePosition;

	position.x -=
		textWidth * 0.5f;

	Transform2DAPI::setPosition(
		m_barkTransform,
		position
	);
}

float BarkManager::estimateTextWidth(
	const std::string& text
) const
{
	if (m_barkText == nullptr)
	{
		return 0.0f;
	}

	float width = 0.0f;

	for (const char character : text)
	{
		switch (character)
		{
			// Spaces
		case ' ':
			width += 4.5f;
			break;

			// Narrow characters
		case 'i':
		case 'l':
		case 'I':
		case 'j':
		case '.':
		case ',':
		case '\'':
		case '!':
		case ':':
		case ';':
			width += 5.5f;
			break;

			// Slightly narrow characters
		case 'f':
		case 't':
		case 'r':
			width += 7.5f;
			break;

			// Wide characters
		case 'W':
		case 'M':
		case 'w':
		case 'm':
			width += 14.0f;
			break;
				
			// Normal characters
		default:
			width += 11.0f;
			break;
		}
	}

	return
		width *
		UITextAPI::getScale(
			m_barkText
		);
}

void BarkManager::interruptCurrentBark()
{
	m_hasCurrentBark = false;
	m_timer = 0.0f;

	clearBarkText();
}

void BarkManager::removeQueuedBarksBelow(
	BarkPriority priority
)
{
	m_barkQueue.erase(
		std::remove_if(
			m_barkQueue.begin(),
			m_barkQueue.end(),
			[priority](const BarkLine& bark)
			{
				return
					static_cast<int>(
						bark.priority
						)
					<
					static_cast<int>(
						priority
						);
			}
		),
		m_barkQueue.end()
	);
}

void BarkManager::pushBarksToFront(
	const std::vector<BarkLine>& barks
)
{
	for (
		auto it = barks.rbegin();
		it != barks.rend();
		++it
		)
	{
		if (it->text.empty())
		{
			continue;
		}

		m_barkQueue.push_front(*it);
	}
}

bool BarkManager::isHigherPriority(
	BarkPriority first,
	BarkPriority second
) const
{
	return
		static_cast<int>(first)
					>
		static_cast<int>(second);
}

const char* BarkManager::getSpeakerName(
	BarkSpeaker speaker
) const
{
	switch (speaker)
	{
	case BarkSpeaker::Death:
		return "Death";

	case BarkSpeaker::Lyriel:
		return "Lyriel";

	default:
		return "Unknown";
	}
}

IMPLEMENT_SCRIPT(BarkManager)
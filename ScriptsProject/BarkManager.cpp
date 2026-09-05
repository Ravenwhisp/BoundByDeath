#include "pch.h"
#include "BarkManager.h"

BarkManager::BarkManager(GameObject* owner)
	: Script(owner)
{
}

void BarkManager::Start()
{
	m_barkText = UITextAPI::getTextComponent(getOwner());

	if (m_barkText == nullptr)
	{
		Debug::warn(
			"BarkManager on '%s' could not find a UIText component.",
			GameObjectAPI::getName(getOwner())
		);
		return;
	}

	UITextAPI::setText(m_barkText, "");
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

void BarkManager::playBarks(const std::vector<BarkLine>& barks)
{
	for (const BarkLine& bark : barks)
	{
		if (bark.text.empty())
		{
			continue;
		}

		m_barkQueue.push(bark);
	}

	if (!m_hasCurrentBark)
	{
		startNextBark();
	}
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

	m_currentBark = m_barkQueue.front();
	m_barkQueue.pop();

	m_timer = m_currentBark.duration;
	m_hasCurrentBark = true;

	showCurrentBark();

	Debug::log(
		"Bark started - %s: %s",
		getSpeakerName(m_currentBark.speaker),
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
		std::string(getSpeakerName(m_currentBark.speaker))
		+ ": "
		+ m_currentBark.text;

	UITextAPI::setText(
		m_barkText,
		displayText.c_str()
	);
}

void BarkManager::clearBarkText()
{
	if (m_barkText == nullptr)
	{
		return;
	}

	UITextAPI::setText(
		m_barkText,
		""
	);
}

const char* BarkManager::getSpeakerName(BarkSpeaker speaker) const
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
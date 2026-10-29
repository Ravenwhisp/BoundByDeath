#include "pch.h"
#include "BarkManager.h"

#include <algorithm>
#include <cctype>

IMPLEMENT_SCRIPT_FIELDS(
	BarkManager,

	FIELD_GROUP_COLLAPSE(
		"Speaker Colors",

		SERIALIZED_VEC3(
			m_lyrielColor,
			"Lyriel"
		),

		SERIALIZED_VEC3(
			m_deathColor,
			"Death"
		),

		SERIALIZED_VEC3(
			m_arthurColor,
			"Arthur"
		),

		SERIALIZED_VEC3(
			m_aelorinColor,
			"Aelorin"
		),

		SERIALIZED_VEC3(
			m_unnamedColor,
			"Unnamed Speaker"
		)
	),

	FIELD_GROUP_COLLAPSE(
		"Readability",

		SERIALIZED_BOOL(
			m_outlineEnabled,
			"Outline"
		),

		SERIALIZED_FLOAT(
			m_outlineSize,
			"Outline Size",
			0.0f,
			2.0f,
			0.05f
		),

		SERIALIZED_BOOL(
			m_shadowEnabled,
			"Shadow"
		),

		SERIALIZED_FLOAT(
			m_shadowOffset,
			"Shadow Offset",
			0.0f,
			8.0f,
			0.5f
		)
	),

	SERIALIZED_FLOAT(
		m_gapBetweenLines,
		"Gap Between Lines",
		0.0f,
		2.0f,
		0.05f
	),

	SERIALIZED_FLOAT(
		m_reactionCooldown,
		"Reaction Cooldown",
		0.0f,
		30.0f,
		0.5f
	),

	SERIALIZED_FLOAT(
		m_startupGrace,
		"Startup Grace",
		0.0f,
		3.0f,
		0.05f
	)
)

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

	m_soundSource =
		AudioAPI::getSoundSourceComponent(getOwner());

	if (m_soundSource == nullptr)
	{
		Debug::warn(
			"BarkManager on '%s' has no sound source: the lines will show without voice.",
			GameObjectAPI::getName(getOwner())
		);
	}

	m_startupTimer = m_startupGrace;

	applyTextStyle();

	clearBarkText();
}

void BarkManager::Update()
{
	if (m_startupTimer > 0.0f)
	{
		m_startupTimer -= Time::getDeltaTime();

		// Lo que llegue mientras tanto se queda en cola, no se pierde.
		return;
	}

	if (m_reactionTimer > 0.0f)
	{
		m_reactionTimer -= Time::getDeltaTime();
	}

	if (m_gapTimer > 0.0f)
	{
		m_gapTimer -= Time::getDeltaTime();

		if (m_gapTimer > 0.0f)
		{
			return;
		}

		m_gapTimer = 0.0f;
	}

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

	// Una reaccion solo vale en su momento: si hay algo hablando o esperando,
	// se descarta en vez de encolarse y salir tarde.
	if (incomingPriority == BarkPriority::Low
		&& (m_hasCurrentBark || !m_barkQueue.empty() || m_reactionTimer > 0.0f))
	{
		return false;
	}

	if (incomingPriority == BarkPriority::Low)
	{
		m_reactionTimer = m_reactionCooldown;
	}

	// Nada corta una linea empezada. Lo urgente se pone delante de la cola y
	// entra en cuanto la actual termina.
	if (m_hasCurrentBark
		&& isHigherPriority(
			incomingPriority,
			m_currentBark.priority
		))
	{
		removeQueuedReactions();

		pushBarksToFront(barks);

		return true;
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
		m_currentBark.speaker.c_str(),
		m_currentBark.text.c_str()
	);
}

void BarkManager::finishCurrentBark()
{
	m_hasCurrentBark = false;
	m_timer = 0.0f;

	clearBarkText();

	// Si queda algo en cola se respeta el respiro; si no, arranca ya y el silencio
	// lo pone la propia ausencia de lineas.
	if (!m_barkQueue.empty() && m_gapBetweenLines > 0.0f)
	{
		m_gapTimer = m_gapBetweenLines;

		return;
	}

	startNextBark();
}

void BarkManager::showCurrentBark()
{
	if (m_barkText == nullptr)
	{
		return;
	}

	const std::string displayText =
		m_currentBark.speaker
		+ ": "
		+ m_currentBark.text;

	const Vector3 color =
		colorForSpeaker(m_currentBark.speaker);

	applyTextStyle();

	UITextAPI::setColor(
		m_barkText,
		color.x,
		color.y,
		color.z,
		1.0f
	);

	UITextAPI::setText(
		m_barkText,
		displayText.c_str()
	);

	playVoice();
}

void BarkManager::playVoice()
{
	stopVoice();

	if (m_soundSource == nullptr
		|| m_currentBark.voiceEvent.empty())
	{
		return;
	}

	m_voicePlayingId =
		AudioAPI::postEvent(
			m_soundSource,
			m_currentBark.voiceBank.c_str(),
			m_currentBark.voiceEvent.c_str()
		);
}

void BarkManager::stopVoice()
{
	if (m_soundSource == nullptr
		|| m_voicePlayingId == 0)
	{
		return;
	}

	AudioAPI::stopEvent(
		m_soundSource,
		m_voicePlayingId
	);

	m_voicePlayingId = 0;
}

void BarkManager::applyTextStyle()
{
	if (m_barkText == nullptr)
	{
		return;
	}

	UITextAPI::setOutline(
		m_barkText,
		m_outlineEnabled,
		m_outlineSize,
		0.0f,
		0.0f,
		0.0f,
		1.0f
	);

	UITextAPI::setShadow(
		m_barkText,
		m_shadowEnabled,
		m_shadowOffset,
		m_shadowOffset,
		0.0f,
		0.0f,
		0.0f,
		0.9f
	);
}

Vector3 BarkManager::colorForSpeaker(
	const std::string& speaker
) const
{
	std::string name = speaker;

	std::transform(
		name.begin(),
		name.end(),
		name.begin(),
		[](unsigned char character)
		{
			return static_cast<char>(
				std::tolower(character)
			);
		}
	);

	if (name == "lyriel")
	{
		return m_lyrielColor;
	}

	if (name == "death")
	{
		return m_deathColor;
	}

	if (name == "arthur")
	{
		return m_arthurColor;
	}

	if (name == "aelorin")
	{
		return m_aelorinColor;
	}

	return m_unnamedColor;
}

void BarkManager::clearBarkText()
{
	// Aqui NO se para la voz. Borrar el texto es cosmetico: si la linea termina sola, el
	// audio tiene que poder acabar su ultima silaba. Lo unico que corta una voz es la
	// siguiente linea, y de eso ya se encarga playVoice().
	if (m_barkText == nullptr)
	{
		return;
	}

	UITextAPI::setText(
		m_barkText,
		""
	);
}


void BarkManager::removeQueuedReactions()
{
	// Solo se descartan las reactivas. Historia y guia no se pierden nunca.
	m_barkQueue.erase(
		std::remove_if(
			m_barkQueue.begin(),
			m_barkQueue.end(),
			[](const BarkLine& bark)
			{
				return bark.priority == BarkPriority::Low;
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

IMPLEMENT_SCRIPT(BarkManager)
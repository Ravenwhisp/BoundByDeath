#include "pch.h"
#include "BarkEventTrigger.h"

#include "BarkEvent.h"

#include <cstring>

static const char* barkTriggerActivationNames[] =
{
	"Both Players Enter",
	"Both Players Stay"
};

constexpr int barkTriggerActivationCount = 2;

IMPLEMENT_SCRIPT_FIELDS(BarkEventTrigger,
	SERIALIZED_ENUM_INT(
		m_activationType,
		"Activation Type",
		barkTriggerActivationNames,
		barkTriggerActivationCount
	),

	SERIALIZED_BOOL(
		m_triggerOnlyOnce,
		"Trigger Only Once"
	),

	SERIALIZED_FLOAT(
		m_delay,
		"Delay",
		0.0f,
		30.0f,
		0.1f
	),

	SERIALIZED_FLOAT(
		m_requiredStayTime,
		"Required Stay Time",
		0.0f,
		60.0f,
		0.1f
	)
)

BarkEventTrigger::BarkEventTrigger(GameObject* owner)
	: Script(owner)
{
}

void BarkEventTrigger::Start()
{
	findPlayers();

	m_barkEvent = GameObjectAPI::findScript<BarkEvent>(getOwner());

	if (m_barkEvent == nullptr)
	{
		Debug::warn(
			"BarkEventTrigger on '%s' could not find a BarkEvent in the same GameObject.",
			GameObjectAPI::getName(getOwner())
		);
	}
}

void BarkEventTrigger::Update()
{
	if (m_barkEvent == nullptr)
	{
		return;
	}

	if (m_triggerOnlyOnce && m_hasTriggered)
	{
		return;
	}

	if (!areBothPlayersInside())
	{
		resetCurrentActivation();
		return;
	}

	if (m_hasTriggeredCurrentOccupancy)
	{
		return;
	}

	switch (static_cast<BarkTriggerActivationType>(m_activationType))
	{
	case BarkTriggerActivationType::BothPlayersEnter:
	{
		if (!m_isWaitingForDelay)
		{
			m_isWaitingForDelay = true;
			m_timer = m_delay;

			if (m_timer <= 0.0f)
			{
				triggerBark();
				return;
			}
		}

		m_timer -= Time::getDeltaTime();

		if (m_timer <= 0.0f)
		{
			triggerBark();
		}
		break;
	}

	case BarkTriggerActivationType::BothPlayersStay:
	{
		m_timer += Time::getDeltaTime();

		if (m_timer >= m_requiredStayTime)
		{
			triggerBark();
		}
		break;
	}

	default:
		break;
	}
}

void BarkEventTrigger::OnTriggerEnter(GameObject* gameObject)
{
	setPlayerInside(gameObject, true);
}

void BarkEventTrigger::OnTriggerExit(GameObject* gameObject)
{
	setPlayerInside(gameObject, false);

	if (!areBothPlayersInside())
	{
		resetCurrentActivation();
	}
}

void BarkEventTrigger::findPlayers()
{
	m_death = nullptr;
	m_lyriel = nullptr;

	m_deathInside = false;
	m_lyrielInside = false;

	const std::vector<GameObject*> players =
		SceneAPI::findAllGameObjectsByTag(Tag::PLAYER, true);

	for (GameObject* player : players)
	{
		if (player == nullptr)
		{
			continue;
		}

		const char* playerName = GameObjectAPI::getName(player);

		if (playerName == nullptr)
		{
			continue;
		}

		if (std::strcmp(playerName, "Death") == 0)
		{
			m_death = player;
		}
		else if (std::strcmp(playerName, "Lyriel") == 0)
		{
			m_lyriel = player;
		}
	}

	if (m_death == nullptr)
	{
		Debug::warn(
			"BarkEventTrigger on '%s' could not find Death.",
			GameObjectAPI::getName(getOwner())
		);
	}

	if (m_lyriel == nullptr)
	{
		Debug::warn(
			"BarkEventTrigger on '%s' could not find Lyriel.",
			GameObjectAPI::getName(getOwner())
		);
	}
}

void BarkEventTrigger::setPlayerInside(GameObject* gameObject, bool inside)
{
	if (gameObject == m_death)
	{
		m_deathInside = inside;
		return;
	}

	if (gameObject == m_lyriel)
	{
		m_lyrielInside = inside;
		return;
	}
}

bool BarkEventTrigger::areBothPlayersInside() const
{
	return m_deathInside && m_lyrielInside;
}

void BarkEventTrigger::triggerBark()
{
	if (m_barkEvent == nullptr)
	{
		return;
	}

	Debug::log(
		"BarkEventTrigger '%s' activated.",
		GameObjectAPI::getName(getOwner())
	);

	m_barkEvent->executeEvent(nullptr);

	m_hasTriggered = true;
	m_hasTriggeredCurrentOccupancy = true;
	m_isWaitingForDelay = false;
	m_timer = 0.0f;
}

void BarkEventTrigger::resetCurrentActivation()
{
	m_timer = 0.0f;
	m_isWaitingForDelay = false;
	m_hasTriggeredCurrentOccupancy = false;
}

IMPLEMENT_SCRIPT(BarkEventTrigger)
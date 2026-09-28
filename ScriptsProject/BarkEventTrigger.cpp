#include "pch.h"
#include "BarkEventTrigger.h"

#include "BarkEvent.h"
#include "Damageable.h"
#include "PlayerState.h"
#include "Bound.h"

#include <cstring>

namespace
{
	const char* barkTriggerActivationNames[] =
	{
		"Both Players Enter",
		"Both Players Stay",
		"Death Enter",
		"Lyriel Enter",
		"Player Takes Damage",
		"Death Knocked Out",
		"Lyriel Knocked Out",
		"First Bound Separation",
		"Repeated Bound Separation"
	};

	constexpr int barkTriggerActivationCount = 9;
}

IMPLEMENT_SCRIPT_FIELDS(
	BarkEventTrigger,

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
	),

	SERIALIZED_COMPONENT_REF(
		m_damageTarget,
		"Damage Target",
		ComponentType::TRANSFORM
	),

	SERIALIZED_COMPONENT_REF(
		m_boundObject,
		"Bound Object",
		ComponentType::TRANSFORM
	)
)

BarkEventTrigger::BarkEventTrigger(GameObject* owner)
	: Script(owner)
{
}

void BarkEventTrigger::Start()
{
	m_barkEvent =
		GameObjectAPI::findScript<BarkEvent>(
			getOwner()
		);

	if (m_barkEvent == nullptr)
	{
		Debug::warn(
			"BarkEventTrigger on '%s' could not find a BarkEvent in the same GameObject.",
			GameObjectAPI::getName(getOwner())
		);

		return;
	}

	const BarkTriggerActivationType activationType =
		static_cast<BarkTriggerActivationType>(
			m_activationType
			);

	switch (activationType)
	{
	case BarkTriggerActivationType::BothPlayersEnter:
	case BarkTriggerActivationType::BothPlayersStay:
	case BarkTriggerActivationType::DeathEnter:
	case BarkTriggerActivationType::LyrielEnter:
		setupZoneTrigger();
		break;

	case BarkTriggerActivationType::PlayerTakesDamage:
		setupDamageTrigger();
		break;

	case BarkTriggerActivationType::DeathKnockedOut:
	case BarkTriggerActivationType::LyrielKnockedOut:
		setupKnockedOutTrigger();
		break;

	case BarkTriggerActivationType::FirstBoundSeparation:
	case BarkTriggerActivationType::RepeatedBoundSeparation:
		setupBoundTrigger();
		break;

	default:
		break;
	}
}

void BarkEventTrigger::Update()
{
	if (m_barkEvent == nullptr)
	{
		return;
	}

	if (
		m_triggerOnlyOnce &&
		m_hasTriggered
		)
	{
		return;
	}

	const BarkTriggerActivationType activationType =
		static_cast<BarkTriggerActivationType>(
			m_activationType
			);

	switch (activationType)
	{
	case BarkTriggerActivationType::BothPlayersEnter:
		updateZoneEnter();
		break;

	case BarkTriggerActivationType::BothPlayersStay:
		updateZoneStay();
		break;

	case BarkTriggerActivationType::DeathEnter:
	case BarkTriggerActivationType::LyrielEnter:
		updatePlayerEnter();
		break;

	case BarkTriggerActivationType::PlayerTakesDamage:
		updateDamage();
		break;

	case BarkTriggerActivationType::DeathKnockedOut:
	case BarkTriggerActivationType::LyrielKnockedOut:
		updateKnockedOut();
		break;

	case BarkTriggerActivationType::FirstBoundSeparation:
	case BarkTriggerActivationType::RepeatedBoundSeparation:
		updateBound();
		break;

	default:
		break;
	}
}

void BarkEventTrigger::OnTriggerEnter(
	GameObject* gameObject
)
{
	const BarkTriggerActivationType activationType =
		static_cast<BarkTriggerActivationType>(
			m_activationType
			);

	if (
		activationType !=
		BarkTriggerActivationType::BothPlayersEnter
		&&
		activationType !=
		BarkTriggerActivationType::BothPlayersStay
		&&
		activationType !=
		BarkTriggerActivationType::DeathEnter
		&&
		activationType !=
		BarkTriggerActivationType::LyrielEnter
		)
	{
		return;
	}

	setPlayerInside(
		gameObject,
		true
	);
}

void BarkEventTrigger::OnTriggerExit(
	GameObject* gameObject
)
{
	const BarkTriggerActivationType activationType =
		static_cast<BarkTriggerActivationType>(
			m_activationType
			);

	if (
		activationType !=
		BarkTriggerActivationType::BothPlayersEnter
		&&
		activationType !=
		BarkTriggerActivationType::BothPlayersStay
		&&
		activationType !=
		BarkTriggerActivationType::DeathEnter
		&&
		activationType !=
		BarkTriggerActivationType::LyrielEnter
		)
	{
		return;
	}

	setPlayerInside(
		gameObject,
		false
	);

	switch (activationType)
	{
	case BarkTriggerActivationType::BothPlayersEnter:
	case BarkTriggerActivationType::BothPlayersStay:
		if (!areBothPlayersInside())
		{
			resetCurrentZoneActivation();
		}
		break;

	case BarkTriggerActivationType::DeathEnter:
	case BarkTriggerActivationType::LyrielEnter:
		if (!isSelectedPlayerInside())
		{
			resetCurrentZoneActivation();
		}
		break;

	default:
		break;
	}
}

void BarkEventTrigger::setupZoneTrigger()
{
	findPlayers();
}

void BarkEventTrigger::setupDamageTrigger()
{
	Transform* targetTransform =
		m_damageTarget.getReferencedComponent();

	if (targetTransform == nullptr)
	{
		Debug::warn(
			"BarkEventTrigger on '%s' has no Damage Target assigned.",
			GameObjectAPI::getName(getOwner())
		);

		return;
	}

	GameObject* targetObject =
		ComponentAPI::getOwner(
			targetTransform
		);

	if (targetObject == nullptr)
	{
		return;
	}

	m_observedDamageable =
		GameObjectAPI::findScript<Damageable>(
			targetObject
		);

	if (m_observedDamageable == nullptr)
	{
		Debug::warn(
			"BarkEventTrigger on '%s' could not find Damageable on Damage Target '%s'.",
			GameObjectAPI::getName(getOwner()),
			GameObjectAPI::getName(targetObject)
		);

		return;
	}

	m_previousHp =
		m_observedDamageable->getCurrentHp();
}

void BarkEventTrigger::setupKnockedOutTrigger()
{
	findPlayers();

	const BarkTriggerActivationType activationType =
		static_cast<BarkTriggerActivationType>(
			m_activationType
			);

	GameObject* targetPlayer = nullptr;

	switch (activationType)
	{
	case BarkTriggerActivationType::DeathKnockedOut:
		targetPlayer = m_death;
		break;

	case BarkTriggerActivationType::LyrielKnockedOut:
		targetPlayer = m_lyriel;
		break;

	default:
		return;
	}

	if (targetPlayer == nullptr)
	{
		Debug::warn(
			"BarkEventTrigger on '%s' could not find the player required by the Knocked Out trigger.",
			GameObjectAPI::getName(getOwner())
		);

		return;
	}

	m_observedPlayerState =
		GameObjectAPI::findScript<PlayerState>(
			targetPlayer
		);

	if (m_observedPlayerState == nullptr)
	{
		Debug::warn(
			"BarkEventTrigger on '%s' could not find PlayerState on '%s'.",
			GameObjectAPI::getName(getOwner()),
			GameObjectAPI::getName(targetPlayer)
		);

		return;
	}

	/*
	 * Initialize the state on the first Update().
	 *
	 * This prevents a bark from firing immediately
	 * if the player already starts in a downed state.
	 */
	m_knockedOutStateInitialized = false;
}

void BarkEventTrigger::setupBoundTrigger()
{
	Transform* boundTransform =
		m_boundObject.getReferencedComponent();

	if (boundTransform == nullptr)
	{
		Debug::warn(
			"BarkEventTrigger on '%s' has no Bound Object assigned.",
			GameObjectAPI::getName(getOwner())
		);

		return;
	}

	GameObject* boundObject =
		ComponentAPI::getOwner(
			boundTransform
		);

	if (boundObject == nullptr)
	{
		return;
	}

	m_bound =
		GameObjectAPI::findScript<Bound>(
			boundObject
		);

	if (m_bound == nullptr)
	{
		Debug::warn(
			"BarkEventTrigger on '%s' could not find Bound on '%s'.",
			GameObjectAPI::getName(getOwner()),
			GameObjectAPI::getName(boundObject)
		);

		return;
	}

	/*
	 * Do not initialize the Bound state here.
	 *
	 * Bound loads its configuration in Start(),
	 * including m_minDistance.
	 *
	 * Since Start() order is not guaranteed,
	 * we initialize the state on the first Update().
	 */
	m_boundStateInitialized = false;
}

void BarkEventTrigger::updateZoneEnter()
{
	if (!areBothPlayersInside())
	{
		resetCurrentZoneActivation();
		return;
	}

	if (m_hasTriggeredCurrentOccupancy)
	{
		return;
	}

	if (!m_isWaitingForDelay)
	{
		m_isWaitingForDelay = true;
		m_timer = m_delay;
	}

	if (m_timer > 0.0f)
	{
		m_timer -=
			Time::getDeltaTime();

		if (m_timer > 0.0f)
		{
			return;
		}
	}

	if (triggerBark())
	{
		m_hasTriggeredCurrentOccupancy = true;
		m_isWaitingForDelay = false;
		m_timer = 0.0f;
	}
}

void BarkEventTrigger::updateZoneStay()
{
	if (!areBothPlayersInside())
	{
		resetCurrentZoneActivation();
		return;
	}

	if (m_hasTriggeredCurrentOccupancy)
	{
		return;
	}

	m_timer +=
		Time::getDeltaTime();

	if (
		m_timer <
		m_requiredStayTime
		)
	{
		return;
	}

	if (triggerBark())
	{
		m_hasTriggeredCurrentOccupancy = true;
		m_timer = 0.0f;
	}
}

void BarkEventTrigger::updatePlayerEnter()
{
	if (!isSelectedPlayerInside())
	{
		resetCurrentZoneActivation();
		return;
	}

	if (m_hasTriggeredCurrentOccupancy)
	{
		return;
	}

	if (!m_isWaitingForDelay)
	{
		m_isWaitingForDelay = true;
		m_timer = m_delay;
	}

	if (m_timer > 0.0f)
	{
		m_timer -=
			Time::getDeltaTime();

		if (m_timer > 0.0f)
		{
			return;
		}
	}

	if (triggerBark())
	{
		m_hasTriggeredCurrentOccupancy = true;
		m_isWaitingForDelay = false;
		m_timer = 0.0f;
	}
}

void BarkEventTrigger::updateDamage()
{
	if (m_observedDamageable == nullptr)
	{
		return;
	}

	const float currentHp =
		m_observedDamageable->getCurrentHp();

	const bool tookDamage =
		currentHp < m_previousHp;

	if (
		tookDamage &&
		!m_observedDamageable->isLastDamageContinuous()
		)
	{
		triggerBark();
	}

	m_previousHp = currentHp;
}

void BarkEventTrigger::updateKnockedOut()
{
	if (m_observedPlayerState == nullptr)
	{
		return;
	}

	const bool isKnockedOut =
		m_observedPlayerState->isDowned();

	/*
	 * The first update only records the initial state.
	 * We only want to react to the transition:
	 *
	 * active -> knocked out
	 */
	if (!m_knockedOutStateInitialized)
	{
		m_wasKnockedOut =
			isKnockedOut;

		m_knockedOutStateInitialized = true;

		return;
	}

	if (
		isKnockedOut &&
		!m_wasKnockedOut
		)
	{
		/*
		 * Only consume the transition if the BarkEvent
		 * was actually accepted by BarkManager.
		 *
		 * If a higher-priority bark temporarily blocks it,
		 * this will retry while the player remains downed.
		 */
		if (triggerBark())
		{
			m_wasKnockedOut = true;
		}

		return;
	}

	/*
	 * When the player is revived this becomes false again,
	 * allowing another KO to trigger a new bark later.
	 */
	m_wasKnockedOut =
		isKnockedOut;
}

void BarkEventTrigger::updateBound()
{
	if (m_bound == nullptr)
	{
		return;
	}

	/*
	 * Initialize here rather than in Start().
	 * At this point Bound should already have loaded
	 * its BoundConfig and m_minDistance.
	 */
	if (!m_boundStateInitialized)
	{
		m_wasSeparated =
			isBoundSeparated();

		m_boundStateInitialized = true;

		return;
	}

	const bool isSeparated =
		isBoundSeparated();

	/*
	 * Only react to:
	 *
	 * inside Bound range
	 *        ->
	 * outside Bound range
	 */
	if (
		isSeparated &&
		!m_wasSeparated
		)
	{
		const BarkTriggerActivationType activationType =
			static_cast<BarkTriggerActivationType>(
				m_activationType
				);

		if (!m_hasSeenFirstSeparation)
		{
			m_hasSeenFirstSeparation = true;

			if (
				activationType ==
				BarkTriggerActivationType::
				FirstBoundSeparation
				)
			{
				triggerBark();
			}
		}
		else
		{
			if (
				activationType ==
				BarkTriggerActivationType::
				RepeatedBoundSeparation
				)
			{
				triggerBark();
			}
		}
	}

	m_wasSeparated =
		isSeparated;
}

void BarkEventTrigger::findPlayers()
{
	m_death = nullptr;
	m_lyriel = nullptr;

	m_deathInside = false;
	m_lyrielInside = false;

	const std::vector<GameObject*> players =
		SceneAPI::findAllGameObjectsByTag(
			Tag::PLAYER,
			true
		);

	for (GameObject* player : players)
	{
		if (player == nullptr)
		{
			continue;
		}

		const char* playerName =
			GameObjectAPI::getName(
				player
			);

		if (playerName == nullptr)
		{
			continue;
		}

		if (
			std::strcmp(
				playerName,
				"Death"
			) == 0
			)
		{
			m_death = player;
		}
		else if (
			std::strcmp(
				playerName,
				"Lyriel"
			) == 0
			)
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

void BarkEventTrigger::setPlayerInside(
	GameObject* gameObject,
	bool inside
)
{
	if (gameObject == m_death)
	{
		m_deathInside = inside;
		return;
	}

	if (gameObject == m_lyriel)
	{
		m_lyrielInside = inside;
	}
}

bool BarkEventTrigger::
areBothPlayersInside() const
{
	return
		m_deathInside &&
		m_lyrielInside;
}

bool BarkEventTrigger::
isSelectedPlayerInside() const
{
	const BarkTriggerActivationType activationType =
		static_cast<BarkTriggerActivationType>(
			m_activationType
			);

	switch (activationType)
	{
	case BarkTriggerActivationType::DeathEnter:
		return m_deathInside;

	case BarkTriggerActivationType::LyrielEnter:
		return m_lyrielInside;

	default:
		return false;
	}
}

bool BarkEventTrigger::
isBoundSeparated() const
{
	if (m_bound == nullptr)
	{
		return false;
	}

	Transform* firstTarget =
		m_bound->
		m_firstTarget.
		getReferencedComponent();

	Transform* secondTarget =
		m_bound->
		m_secondTarget.
		getReferencedComponent();

	if (
		firstTarget == nullptr ||
		secondTarget == nullptr
		)
	{
		return false;
	}

	const Vector3 firstPosition =
		TransformAPI::getGlobalPosition(
			firstTarget
		);

	const Vector3 secondPosition =
		TransformAPI::getGlobalPosition(
			secondTarget
		);

	const float distance =
		Vector3::Distance(
			firstPosition,
			secondPosition
		);

	return
		distance >
		m_bound->m_minDistance;
}

bool BarkEventTrigger::triggerBark()
{
	if (m_barkEvent == nullptr)
	{
		return false;
	}

	const bool played =
		m_barkEvent->play();

	if (!played)
	{
		return false;
	}

	Debug::log(
		"BarkEventTrigger '%s' activated.",
		GameObjectAPI::getName(getOwner())
	);

	if (m_triggerOnlyOnce)
	{
		m_hasTriggered = true;
	}

	return true;
}

void BarkEventTrigger::
resetCurrentZoneActivation()
{
	m_timer = 0.0f;
	m_isWaitingForDelay = false;
	m_hasTriggeredCurrentOccupancy = false;
}

IMPLEMENT_SCRIPT(BarkEventTrigger)
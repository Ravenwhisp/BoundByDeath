#include "pch.h"
#include "TriggerFire.h"

#include "AmbientSoundLoop.h"
#include "EnvironmentSound.h"

IMPLEMENT_SCRIPT_FIELDS(TriggerFire,
	SERIALIZED_COMPONENT_REF(m_fireEffectT, "Fire Effect", ComponentType::TRANSFORM),
	SERIALIZED_COMPONENT_REF(m_lightT, "Light", ComponentType::TRANSFORM)
)

TriggerFire::TriggerFire(GameObject* owner)
	: Script(owner)
{
}

void TriggerFire::Start()
{
	m_fireTriggered = false;

	Transform* fireEffectTransform = m_fireEffectT.getReferencedComponent();
	if(fireEffectTransform != nullptr)
	{
		GameObject* fireEffectGO = ComponentAPI::getOwner(fireEffectTransform);
		GameObjectAPI::setActive(fireEffectGO, false);
	}

	Transform* lightTransform = m_lightT.getReferencedComponent();
	if(lightTransform != nullptr)
	{
		GameObject* lightGO = ComponentAPI::getOwner(lightTransform);
		GameObjectAPI::setActive(lightGO, false);
	}
}

void TriggerFire::OnTriggerEnter(GameObject* gameObject)
{
	if (gameObject == nullptr || GameObjectAPI::getTag(gameObject) != Tag::PLAYER)
	{
		Debug::log("TriggerFire: Non-player object entered trigger, ignoring.");
		return;
	}
	if (!m_fireTriggered)
	{
		triggerFire();
	}
	else
	{
		Debug::log("TriggerFire: Fire already triggered, ignoring trigger.");
		return;
	}
}

void TriggerFire::Update()
{
	if (!m_soundPending)
	{
		return;
	}

	m_soundDelay -= Time::getDeltaTime();
	if (m_soundDelay > 0.0f)
	{
		return;
	}

	Transform* fireEffectTransform = m_fireEffectT.getReferencedComponent();
	GameObject* fireEffectGO = fireEffectTransform != nullptr ? ComponentAPI::getOwner(fireEffectTransform) : nullptr;
	if (fireEffectGO == nullptr)
	{
		m_soundPending = false;
		return;
	}

	m_soundDelay = 0.5f;
	--m_soundRetries;

	const uint32_t playingID = EnvironmentSound::play(fireEffectGO, "Play_Environment_Fire_Ignite");
	if (playingID == 0 && m_soundRetries > 0)
	{
		return;
	}

	m_soundPending = false;

	if (AmbientSoundLoop* loop = GameObjectAPI::findScript<AmbientSoundLoop>(fireEffectGO))
	{
		loop->play();
	}
}

void TriggerFire::triggerFire()
{
	m_fireTriggered = true;

	Transform* fireEffectTransform = m_fireEffectT.getReferencedComponent();
	if(fireEffectTransform != nullptr)
	{
		GameObject* fireEffectGO = ComponentAPI::getOwner(fireEffectTransform);
		GameObjectAPI::setActive(fireEffectGO, true);

		// The emitter is only known to Wwise once the object has been active for a frame,
		// so the ignite cannot be posted here, in the frame the torch lights up.
		m_soundPending = true;
		m_soundDelay = 0.1f;
		m_soundRetries = 10;
	}
	Transform* lightTransform = m_lightT.getReferencedComponent();
	if(lightTransform != nullptr)
	{
		GameObject* lightGO = ComponentAPI::getOwner(lightTransform);
		GameObjectAPI::setActive(lightGO, true);
	}
}

IMPLEMENT_SCRIPT(TriggerFire)

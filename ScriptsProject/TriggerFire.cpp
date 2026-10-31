#include "pch.h"
#include "TriggerFire.h"

#include "AmbientSoundLoop.h"
#include "EnvironmentSound.h"

IMPLEMENT_SCRIPT_FIELDS(TriggerFire,
	SERIALIZED_COMPONENT_REF(m_fireEffectT, "Fire Effect", ComponentType::TRANSFORM),
	SERIALIZED_COMPONENT_REF(m_lightT, "Light", ComponentType::TRANSFORM),
	SERIALIZED_COMPONENT_REF_VECTOR(m_soundEmitters, "Sound Emitters", ComponentType::TRANSFORM)
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

	std::vector<GameObject*> emitters;

	for (const ComponentRef<Transform>& ref : m_soundEmitters)
	{
		Transform* emitterTransform = ref.getReferencedComponent();
		if (emitterTransform != nullptr)
		{
			emitters.push_back(ComponentAPI::getOwner(emitterTransform));
		}
	}

	if (emitters.empty())
	{
		Transform* fireEffectTransform = m_fireEffectT.getReferencedComponent();
		if (fireEffectTransform != nullptr)
		{
			emitters.push_back(ComponentAPI::getOwner(fireEffectTransform));
		}
	}

	if (emitters.empty())
	{
		m_soundPending = false;
		return;
	}

	m_soundDelay = 0.5f;
	--m_soundRetries;

	if (EnvironmentSound::play(emitters[0], "Play_Environment_Fire_Ignite") == 0 && m_soundRetries > 0)
	{
		return;
	}

	m_soundPending = false;

	for (size_t i = 1; i < emitters.size(); ++i)
	{
		EnvironmentSound::play(emitters[i], "Play_Environment_Fire_Ignite");
	}

	for (GameObject* emitter : emitters)
	{
		if (AmbientSoundLoop* loop = GameObjectAPI::findScript<AmbientSoundLoop>(emitter))
		{
			loop->play();
		}
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

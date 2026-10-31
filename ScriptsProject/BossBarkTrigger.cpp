#include "pch.h"
#include "BossBarkTrigger.h"

#include "AelorinBossController.h"
#include "ArthurBossController.h"
#include "BarkEvent.h"
#include "Damageable.h"

namespace
{
	const char* bossBarkConditionNames[] =
	{
		"Encounter Time",
		"Health Percent Below",
		"Animation State Enter",
		"Phase Enter"
	};

	constexpr int bossBarkConditionCount = 4;

	const char* bossBarkPhaseFilterNames[] =
	{
		"Any",
		"Phase 1",
		"Phase 2"
	};

	constexpr int bossBarkPhaseFilterCount = 3;
}

IMPLEMENT_SCRIPT_FIELDS(
	BossBarkTrigger,

	SERIALIZED_ENUM_INT(
		m_conditionType,
		"Bark Condition",
		bossBarkConditionNames,
		bossBarkConditionCount
	),

	SERIALIZED_COMPONENT_REF(
		m_bossObject,
		"Boss Object",
		ComponentType::TRANSFORM
	),

	SERIALIZED_ENUM_INT(
		m_phaseFilter,
		"Phase Filter",
		bossBarkPhaseFilterNames,
		bossBarkPhaseFilterCount
	),

	FIELD_GROUP_COLLAPSE(
		"Condition Settings",

		SERIALIZED_FLOAT(
			m_encounterTime,
			"Encounter Time",
			0.0f,
			600.0f,
			0.1f
		),

		SERIALIZED_FLOAT(
			m_healthPercent,
			"Health Percent",
			0.0f,
			1.0f,
			0.01f
		),

		SERIALIZED_STRING(
			m_animationStateName,
			"Animation State"
		),

		SERIALIZED_INT(
			m_requiredOccurrence,
			"Required Occurrence"
		)
	),

	SERIALIZED_FLOAT(
		m_barkDelay,
		"Bark Delay",
		0.0f,
		30.0f,
		0.1f
	),

	SERIALIZED_BOOL(
		m_barkOnlyOnce,
		"Bark Only Once"
	)
)

BossBarkTrigger::BossBarkTrigger(GameObject* owner)
	: Script(owner)
{
}

void BossBarkTrigger::Start()
{
	m_barkEvent = GameObjectAPI::findScript<BarkEvent>(getOwner());

	if (m_barkEvent == nullptr)
	{
		Debug::warn(
			"BossBarkTrigger on '%s' could not find a BarkEvent.",
			GameObjectAPI::getName(getOwner())
		);
	}

	Transform* bossTransform = m_bossObject.getReferencedComponent();

	if (bossTransform == nullptr)
	{
		Debug::warn(
			"BossBarkTrigger on '%s' has no Boss Object assigned.",
			GameObjectAPI::getName(getOwner())
		);

		return;
	}

	m_boss = ComponentAPI::getOwner(bossTransform);

	if (m_boss == nullptr)
	{
		return;
	}

	// findScript hace dynamic_cast, asi que pedir Damageable devuelve el de cada jefe.
	m_damageable = GameObjectAPI::findScript<Damageable>(m_boss);
	m_aelorin = GameObjectAPI::findScript<AelorinBossController>(m_boss);
	m_arthur = GameObjectAPI::findScript<ArthurBossController>(m_boss);

	if (resolveAnimation() == nullptr)
	{
		Debug::warn(
			"BossBarkTrigger on '%s': the Boss Object has no state machine, so Animation State Enter will never fire.",
			GameObjectAPI::getName(getOwner())
		);
	}

	m_previousPhase = currentPhase();
}

void BossBarkTrigger::Update()
{
	if (m_waitingForDelay)
	{
		m_delayTimer -= Time::getDeltaTime();

		if (m_delayTimer <= 0.0f)
		{
			// Si la linea esta ocupada, playBark no suena y se sigue esperando: se
			// reintenta cada frame hasta que el bloque anterior termine.
			if (playBark())
			{
				m_waitingForDelay = false;
			}
		}

		return;
	}

	if (m_barkOnlyOnce && m_hasPlayed)
	{
		return;
	}

	if (m_boss == nullptr)
	{
		return;
	}

	const bool encounterActive = isEncounterActive();

	if (encounterActive && !m_encounterWasActive)
	{
		m_encounterTimer = 0.0f;
	}

	m_encounterWasActive = encounterActive;

	if (!encounterActive)
	{
		return;
	}

	m_encounterTimer += Time::getDeltaTime();

	if (evaluateCondition())
	{
		requestBark();
	}
}

bool BossBarkTrigger::isEncounterActive() const
{
	if (m_aelorin != nullptr)
	{
		return m_aelorin->hasEncounterStarted();
	}

	// Arthur no expone el arranque del encuentro: el primer dano sirve de senal.
	if (m_damageable != nullptr)
	{
		return m_damageable->getCurrentHp() < m_damageable->getMaxHp();
	}

	return true;
}

int BossBarkTrigger::currentPhase() const
{
	if (m_aelorin != nullptr)
	{
		return static_cast<int>(m_aelorin->getPhase());
	}

	if (m_arthur != nullptr)
	{
		return static_cast<int>(m_arthur->getPhase());
	}

	return -1;
}

bool BossBarkTrigger::passesPhaseFilter() const
{
	if (m_phaseFilter == static_cast<int>(BossBarkPhaseFilter::Any))
	{
		return true;
	}

	const int phase = currentPhase();

	if (phase < 0)
	{
		return false;
	}

	// Phase1 del filtro es el indice 1, y la fase 0 del jefe. De ahi el menos uno.
	return phase == m_phaseFilter - 1;
}

AnimationComponent* BossBarkTrigger::resolveAnimation() const
{
	if (m_boss == nullptr)
	{
		return nullptr;
	}

	// Primero los hijos activos. Aelorin tiene una maquina de estados en cada uno de sus
	// dos hijos de fase y solo una esta encendida cada vez, asi que cachear el puntero en
	// Start dejaba leyendo la de la fase 1 durante toda la fase 2.
	Transform* bossTransform =
		GameObjectAPI::getTransform(m_boss);

	if (bossTransform != nullptr)
	{
		const int childCount =
			TransformAPI::getChildCount(bossTransform);

		for (
			int i = 0;
			i < childCount;
			++i
			)
		{
			Transform* childTransform =
				TransformAPI::getChild(bossTransform, i);

			GameObject* child =
				childTransform != nullptr
				? ComponentAPI::getOwner(childTransform)
				: nullptr;

			if (child == nullptr
				|| !GameObjectAPI::isActiveInHierarchy(child))
			{
				continue;
			}

			AnimationComponent* childAnimation =
				AnimationAPI::getAnimationComponent(child);

			if (childAnimation != nullptr
				&& AnimationAPI::hasStateMachine(childAnimation))
			{
				return childAnimation;
			}
		}
	}

	// Y si ningun hijo la lleva, el propio jefe: es el caso de Arthur.
	AnimationComponent* own =
		AnimationAPI::getAnimationComponent(m_boss);

	if (own != nullptr
		&& AnimationAPI::hasStateMachine(own))
	{
		return own;
	}

	return nullptr;
}

const char* BossBarkTrigger::activeStateName() const
{
	AnimationComponent* animation =
		resolveAnimation();

	if (animation == nullptr)
	{
		return nullptr;
	}

	if (!AnimationAPI::hasActiveState(animation))
	{
		return nullptr;
	}

	return AnimationAPI::getActiveStateName(animation);
}

bool BossBarkTrigger::evaluateCondition()
{
	switch (static_cast<BossBarkConditionType>(m_conditionType))
	{
	case BossBarkConditionType::EncounterTime:
	{
		return passesPhaseFilter()
			&& m_encounterTimer >= m_encounterTime;
	}

	case BossBarkConditionType::HealthPercentBelow:
	{
		if (m_damageable == nullptr || !passesPhaseFilter())
		{
			return false;
		}

		return m_damageable->getHpPercent() <= m_healthPercent;
	}

	case BossBarkConditionType::AnimationStateEnter:
	{
		const char* state = activeStateName();
		const std::string current = state != nullptr ? state : "";

		const bool entered =
			current != m_previousStateName
			&& !m_animationStateName.empty()
			&& current == m_animationStateName;

		m_previousStateName = current;

		// La ocurrencia se cuenta solo dentro de la fase filtrada: Aelorin repite el mismo
		// nombre de estado en los dos statemachines.
		if (entered && passesPhaseFilter())
		{
			++m_stateEnterCount;
			return m_stateEnterCount == m_requiredOccurrence;
		}

		return false;
	}

	case BossBarkConditionType::PhaseEnter:
	{
		const int phase = currentPhase();
		const bool changed = phase >= 0 && phase != m_previousPhase;

		m_previousPhase = phase;

		return changed && passesPhaseFilter();
	}
	}

	return false;
}

void BossBarkTrigger::requestBark()
{
	// Siempre por la via de espera, aunque el retardo sea cero: asi un bloque que llegue
	// con la linea ocupada se reintenta en el siguiente frame en vez de perder su turno.
	m_waitingForDelay = true;
	m_delayTimer = m_barkDelay;
}

bool BossBarkTrigger::playBark()
{
	if (m_barkEvent == nullptr)
	{
		return false;
	}

	// Dos bloques de conversacion no pueden solaparse nunca: si la linea esta ocupada se
	// deja para el siguiente frame en vez de encolarse, porque encolado se intercala con
	// lo que ya hubiera y una pregunta se queda sin su respuesta. El flanco no se pierde:
	// m_hasPlayed solo se marca cuando de verdad ha sonado.
	if (m_barkEvent->isBarkLineBusy())
	{
		return false;
	}

	// Solo se consume si ha sonado: play() devuelve false con el cooldown corriendo, con el
	// trigger chance fallado o sin BarkManager en la escena.
	if (m_barkEvent->play())
	{
		m_hasPlayed = true;

		return true;
	}

	return false;
}

IMPLEMENT_SCRIPT(BossBarkTrigger)

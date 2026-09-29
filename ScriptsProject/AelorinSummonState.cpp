#include "pch.h"
#include "AelorinSummonState.h"

#include "AelorinAttackConfig.h"
#include "AelorinSummonSlot.h"
#include "AelorinVFX.h"

#include <algorithm>

AelorinSummonState::AelorinSummonState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void AelorinSummonState::OnStateEnter()
{
	Transform* parentTransform = TransformAPI::getParent(getOwner()->GetTransform());
	if (!parentTransform)
	{
		Debug::error("[AelorinSummonState] Aelorin transform not found.");
		return;
	}

	GameObject* parentGameObject = ComponentAPI::getOwner(parentTransform);

	// get scripts
	m_controller = GameObjectAPI::findScript<AelorinBossController>(parentGameObject);
	m_animation = AnimationAPI::getAnimationComponent(getOwner());
	m_vfx = GameObjectAPI::findScript<AelorinVFX>(parentGameObject);

	// reset members
	m_activeAbility = AelorinAbility::None;
	m_stateTimer = 0.0f;
	m_recoveryTimer = 0.0f;
	m_summonExecuted = false;
	m_completed = false;
	m_pendingSummonSlots.clear();

	if (!m_controller)
	{
		Debug::error("[AelorinSummonState] AelorinBossController not found.");
		return;
	}

	if (!m_animation)
	{
		Debug::error("[AelorinSummonState] AnimationComponent not found.");
		return;
	}

	if (!m_vfx)
	{
		Debug::error("[AelorinSummonState] AelorinVFX not found.");
	}

	// consume ability
	m_activeAbility = m_controller->consumeRequestedAbility();
	if (m_activeAbility != AelorinAbility::Summon)
	{
		Debug::warn("[AelorinSummonState] Unexpected requested ability!");
		return;
	}

	if (m_vfx && !m_controller->isPhase2())
	{
		m_vfx->startPhase1Spell();
	}

	const Vector3 lyrielPosition = m_controller->getLyrielPosition();
	const Vector3 deathPosition = m_controller->getDeathPosition();

	const Vector3 middlePosition = (lyrielPosition + deathPosition) * 0.5f;

	m_controller->facePositionInstant(middlePosition);

	prepareSummons();

	Debug::log("[AelorinSummonState] ENTER");
}

void AelorinSummonState::OnStateUpdate()
{
	if (!m_controller || !m_animation || m_completed)
	{
		return;
	}

	if (m_controller->trySendPriorityInterrupt(m_animation))
	{
		return;
	}

	const AelorinAttackConfig* config = m_controller->getAelorinAttackConfig();
	if (!config)
	{
		return;
	}

	if (!m_summonExecuted)
	{
		m_stateTimer += Time::getDeltaTime();

		if (m_stateTimer < config->m_summonCastDuration)
		{
			return;
		}

		executeSummon();

		m_summonExecuted = true;
		return;
	}

	m_recoveryTimer += Time::getDeltaTime();

	if (m_recoveryTimer < config->m_summonRecoveryDuration)
	{
		return;
	}

	if (m_vfx && !m_controller->isPhase2())
	{
		m_vfx->stopPhase1Spell();
	}

	finishAbility();
}

void AelorinSummonState::OnStateExit()
{
	if (m_vfx)
	{
		m_vfx->stopPhase1Spell();
	}

	m_pendingSummonSlots.clear();
	m_vfx = nullptr;
	m_stateTimer = 0.0f;
	m_recoveryTimer = 0.0f;
	m_summonExecuted = false;
	m_completed = false;
	Debug::log("[AelorinSummonState] EXIT");
}

void AelorinSummonState::executeSummon()
{
	if (!m_controller)
	{
		return;
	}

	int spawnedCount = 0;

	for (AelorinSummonSlot* slot : m_pendingSummonSlots)
	{
		if (!slot)
		{
			continue;
		}

		GameObject* enemy = slot->spawnEnemy();
		if (!enemy)
		{
			continue;
		}

		++spawnedCount;
	}

	if (spawnedCount > 0)
	{
		m_controller->startSummonTimer();
	}

	Debug::log("[AelorinSummonState] Summoned %d prepared enemies.", spawnedCount);

	m_pendingSummonSlots.clear();
}

void AelorinSummonState::prepareSummons()
{
	m_pendingSummonSlots.clear();

	if (!m_controller)
	{
		return;
	}

	const int livingCount =	m_controller->getLivingSummonCount();
	const int cap =	m_controller->getCurrentSummonCap();
	const int missing =	(std::max)(0, cap - livingCount);

	if (missing <= 0)
	{
		Debug::log("[AelorinSummonState] No enemies need to be summoned.");
		return;
	}

	Transform* formationRoot = m_controller->isPhase2() ? m_controller->getPhase2SummonFormation() : m_controller->getPhase1SummonFormation();
	if (!formationRoot)
	{
		Debug::warn("[AelorinSummonState] Summon formation not assigned.");
		return;
	}

	const int slotCount = TransformAPI::getChildCount(formationRoot);

	for (int i = 0;	i < slotCount && static_cast<int>(m_pendingSummonSlots.size()) < missing; ++i)
	{
		Transform* slotTransform = TransformAPI::getChild(formationRoot, i);
		if (!slotTransform)
		{
			continue;
		}

		GameObject* slotObject = ComponentAPI::getOwner(slotTransform);
		if (!slotObject)
		{
			continue;
		}

		AelorinSummonSlot* slot = GameObjectAPI::findScript<AelorinSummonSlot>(slotObject);
		if (!slot)
		{
			continue;
		}

		if (slot->hasLivingEnemy())
		{
			continue;
		}

		m_pendingSummonSlots.push_back(slot);

		if (m_vfx)
		{
			const Vector3 position = TransformAPI::getGlobalPosition(slotTransform);
			m_vfx->playSummonEnemyEffect(position);
		}
	}

	Debug::log("[AelorinSummonState] Prepared %d summon slots.", static_cast<int>(m_pendingSummonSlots.size()));
}

void AelorinSummonState::finishAbility()
{
	if (m_completed)
	{
		return;
	}

	m_completed = true;

	const bool sent = AnimationAPI::sendTrigger(m_animation, "ToIdle");
	if (!sent)
	{
		Debug::warn("[AelorinSummonState] Failed to send ToIdle trigger.");
	}
}

IMPLEMENT_SCRIPT(AelorinSummonState)
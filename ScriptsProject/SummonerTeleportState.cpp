#include "pch.h"
#include "SummonerTeleportState.h"

#include "SummonerEnemyController.h"
#include "SummonerParticles.h"

SummonerTeleportState::SummonerTeleportState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void SummonerTeleportState::OnStateEnter()
{
	m_controller = GameObjectAPI::findScript<SummonerEnemyController>(getOwner());
	m_animation = AnimationAPI::getAnimationComponent(getOwner());
	m_particles = GameObjectAPI::findScript<SummonerParticles>(getOwner());

	if (!m_controller)
	{
		Debug::error("[SummonerTeleportState] EnemyController not found.");
		return;
	}

	if (!m_animation)
	{
		Debug::error("[SummonerTeleportState] AnimationComponent not found.");
		return;
	}

	if (!m_particles)
	{
		Debug::warn("[SummonerTeleportState] SummonerParticles not found.");
	}

	Debug::log("[SummonerTeleportState] ENTER");

	if (m_controller->isForcedMovementActive())
	{
		AnimationAPI::sendTrigger(m_animation, "ToIdle");
		return;
	}

	Vector3 teleportPosition;
	if (m_controller->tryGetTeleportPosition(teleportPosition))
	{
		Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());

		if (ownerTransform)
		{
			const Vector3 previousPosition = TransformAPI::getGlobalPosition(ownerTransform);

			if (m_particles)
			{
				m_particles->playTeleportParticle(previousPosition);
			}

			TransformAPI::setGlobalPosition(ownerTransform, teleportPosition);

			if (m_particles)
			{
				m_particles->playTeleportParticle(teleportPosition);
			}

			m_controller->consumeTeleportCooldown();

			Debug::log("[SummonerTeleportState] Teleported.");
		}
	}
	else
	{
		Debug::warn("[SummonerTeleportState] No valid teleport position found.");
	}

	AnimationAPI::sendTrigger(m_animation, "ToIdle");
}

void SummonerTeleportState::OnStateUpdate()
{
}

void SummonerTeleportState::OnStateExit()
{
	Debug::log("[SummonerTeleportState] EXIT");
}

IMPLEMENT_SCRIPT(SummonerTeleportState)
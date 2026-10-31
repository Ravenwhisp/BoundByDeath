#include "pch.h"
#include "AelorinDeathState.h"

#include "AelorinSound.h"

#include "AelorinBossController.h"

namespace
{
    // Los estados cuelgan de los hijos Phase1 y Phase2; el script de sonido vive en el
    // GameObject padre, igual que el controller.
    AelorinSound* aelorinSound(GameObject* stateOwner)
    {
        Transform* parentTransform = TransformAPI::getParent(stateOwner->GetTransform());

        if (!parentTransform)
        {
            return nullptr;
        }

        return GameObjectAPI::findScript<AelorinSound>(
            ComponentAPI::getOwner(parentTransform));
    }
}

AelorinDeathState::AelorinDeathState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void AelorinDeathState::OnStateEnter()
{
	if (AelorinSound* sound = aelorinSound(getOwner()))
	{
		sound->stopAllLoops();
	}

	if (AelorinSound* sound = aelorinSound(getOwner()))
	{
		sound->playDeathRoar();
	}

	Transform* parentTransform = TransformAPI::getParent(getOwner()->GetTransform());
	if (!parentTransform)
	{
		Debug::error("[AelorinDeathState] Aelorin transform not found.");
		return;
	}

	GameObject* parentGameObject = ComponentAPI::getOwner(parentTransform);

	m_controller = GameObjectAPI::findScript<AelorinBossController>(parentGameObject);
	m_animation = AnimationAPI::getAnimationComponent(getOwner());

	m_animationFinished = false;

	if (!m_controller)
	{
		Debug::error("[AelorinDeathState] AelorinBossController not found.");
	}

	if (!m_animation)
	{
		Debug::error("[AelorinDeathState] AnimationComponent not found.");
	}

	if (m_controller)
	{
		m_controller->clearPath();
		m_controller->resetRepathTimer();

		// Nothing should move Aelorin anymore.
		m_controller->setForcedMovementActive(false);
		m_controller->setForcedMovementBlocked(true);
	}

	AnimationAPI::setSpeedMultiplier(m_animation, 1.0f);

	Debug::log("[AelorinDeathState] ENTER");
}

void AelorinDeathState::OnStateUpdate()
{
	if (!m_animation || m_animationFinished)
	{
		return;
	}

	if (!AnimationAPI::isPlaying(m_animation))
	{
		m_animationFinished = true;
		Debug::log("[AelorinDeathState] Death animation finished.");

		if (m_controller)
		{
			m_controller->onDefeat();
		}
	}
}

void AelorinDeathState::OnStateExit()
{
	if (m_animation)
	{
		AnimationAPI::setSpeedMultiplier(m_animation, 1.0f);
	}
	// this should normally never happen.
	Debug::log("[AelorinDeathState] EXIT");
}

IMPLEMENT_SCRIPT(AelorinDeathState)
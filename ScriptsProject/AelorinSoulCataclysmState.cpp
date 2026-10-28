#include "pch.h"
#include "AelorinSoulCataclysmState.h"

#include "AelorinSound.h"

#include "AelorinBossController.h"
#include "AelorinAttackConfig.h"
#include "AelorinAttackExecutor.h"
#include "AelorinUI.h"
#include "AelorinLavaController.h"
#include "CameraShake.h"

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

AelorinSoulCataclysmState::AelorinSoulCataclysmState(GameObject* owner)
	: StateMachineScript(owner)
{
}

void AelorinSoulCataclysmState::OnStateEnter()
{
	if (AelorinSound* sound = aelorinSound(getOwner()))
	{
		sound->startCataclysmChannel();
	}

	Transform* parentTransform = TransformAPI::getParent(getOwner()->GetTransform());
	if (!parentTransform)
	{
		Debug::error("[AelorinSoulCataclysmState] Aelorin transform not found.");
		return;
	}

	GameObject* parentGameObject = ComponentAPI::getOwner(parentTransform);
	GameObject* cameraObject = SceneAPI::getDefaultCameraGameObject();

	// get scripts
	m_controller = GameObjectAPI::findScript<AelorinBossController>(parentGameObject);
	m_animation = AnimationAPI::getAnimationComponent(getOwner());
	m_aelorinUI = GameObjectAPI::findScript<AelorinUI>(parentGameObject);
	m_lavaController = GameObjectAPI::findScript<AelorinLavaController>(parentGameObject);
	m_cameraShake = GameObjectAPI::findScript<CameraShake>(cameraObject);

	// reset members
	m_stateTimer = 0.0f;
	m_cataclysmExecuted = false;
	m_completed = false;

	if (!m_controller)
	{
		Debug::error("[AelorinSoulCataclysmState] AelorinBossController not found.");
		return;
	}

	if (!m_animation)
	{
		Debug::error("[AelorinSoulCataclysmState] AnimationComponent not found.");
		return;
	}

	if (!m_aelorinUI)
	{
		Debug::error("[AelorinSoulCataclysmState] AelorinUI not found.");
	}

	if (!m_lavaController)
	{
		Debug::error("[AelorinSoulCataclysmState] AelorinLavaController not found.");
	}

	if (!m_cameraShake)
	{
		Debug::warn("[AelorinSoulCataclysmState] CameraShake not found.");
	}

	AnimationAPI::setSpeedMultiplier(m_animation, 1.0f);

	m_attackExecutor = m_controller->getAttackExecutor();

	if (!m_attackExecutor)
	{
		Debug::error("[AelorinSoulCataclysmState] AelorinAttackExecutor not found.");
		return;
	}

	const AelorinAttackConfig* config = m_controller->getAelorinAttackConfig();
	if (!config)
	{
		Debug::error("[AelorinSoulCataclysmState] Attack Config not found.");
		return;
	}

	Transform* cataclysmCenter = m_controller->getSoulCataclysmCenter();
	Transform* safeZonesRoot = m_controller->getTeleportAnchorsRoot();

	if (!cataclysmCenter)
	{
		Debug::warn("[AelorinSoulCataclysmState] Soul Cataclysm Center not assigned.");
		return;
	}

	if (!safeZonesRoot)
	{
		Debug::warn("[AelorinSoulCataclysmState] Teleport Anchors Root not assigned.");
		return;
	}

	if (m_aelorinUI)
	{
		const Vector3 center = TransformAPI::getGlobalPosition(cataclysmCenter);

		m_aelorinUI->showSoulCataclysmUI(
			safeZonesRoot,
			config->m_soulCataclysmSafeZoneRadius,
			config->m_soulCataclysmChannelDuration
		);
	}

	if (m_lavaController)
	{
		m_lavaController->StartLavaRise(3.116f, config->m_soulCataclysmChannelDuration);
	}

	if (m_cameraShake)
	{
		m_cameraShake->shake(0.05f, config->m_soulCataclysmChannelDuration);
	}

	Debug::log("[AelorinSoulCataclysmState] ENTER");
}

void AelorinSoulCataclysmState::OnStateUpdate()
{
	if (!m_controller || !m_attackExecutor || !m_animation || m_completed)
	{
		return;
	}

	const AelorinAttackConfig* config = m_controller->getAelorinAttackConfig();
	if (!config)
	{
		return;
	}

	m_stateTimer += Time::getDeltaTime();

	if (!m_cataclysmExecuted && m_stateTimer >= config->m_soulCataclysmChannelDuration)
	{
		executeCataclysm();
		m_cataclysmExecuted = true;
		finishCataclysm();
	}
}

void AelorinSoulCataclysmState::OnStateExit()
{
	if (AelorinSound* sound = aelorinSound(getOwner()))
	{
		sound->stopCataclysmChannel();
	}

	if (m_animation)
	{
		AnimationAPI::setSpeedMultiplier(m_animation, 1.0f);
	}

	m_aelorinUI = nullptr;
	m_stateTimer = 0.0f;
	m_cataclysmExecuted = false;
	m_completed = false;

	Debug::log("[AelorinSoulCataclysmState] EXIT");
}

void AelorinSoulCataclysmState::executeCataclysm()
{
	if (AelorinSound* sound = aelorinSound(getOwner()))
	{
		sound->playCataclysmDetonate();
	}

	if (!m_controller)
	{
		return;
	}

	damagePlayerIfUnsafe(m_controller->getLyrielTransform());
	damagePlayerIfUnsafe(m_controller->getDeathTransform());

	Debug::log("[AelorinSoulCataclysmState] SOUL CATACLYSM!");
}

void AelorinSoulCataclysmState::damagePlayerIfUnsafe(Transform* playerTransform)
{
	if (!playerTransform || !m_controller || !m_attackExecutor)
	{
		return;
	}

	if (!m_attackExecutor->isValidDamageTarget(playerTransform))
	{
		return;
	}

	const AelorinAttackConfig* config =	m_controller->getAelorinAttackConfig();
	if (!config)
	{
		return;
	}

	Transform* cataclysmCenter = m_controller->getSoulCataclysmCenter();
	if (!cataclysmCenter)
	{
		return;
	}

	if (isPlayerInsideSafeZone(playerTransform))
	{
		Debug::log("[AelorinSoulCataclysmState] Player is inside a safe zone");
		return;
	}

	const Vector3 center = TransformAPI::getGlobalPosition(cataclysmCenter);

	m_attackExecutor->tryDamageTargetInRadius(playerTransform, center, config->m_soulCataclysmRadius, config->m_soulCataclysmDamage, "Aelorin Soul Cataclysm");
}

bool AelorinSoulCataclysmState::isPlayerInsideSafeZone(Transform* playerTransform) const
{
	if (!playerTransform || !m_controller)
	{
		return false;
	}

	Transform* safeZonesRoot = m_controller->getTeleportAnchorsRoot();
	if (!safeZonesRoot)
	{
		return false;
	}

	const AelorinAttackConfig* config = m_controller->getAelorinAttackConfig();
	if (!config)
	{
		return false;
	}

	const Vector3 playerPosition = TransformAPI::getGlobalPosition(playerTransform);
	const float safeRadiusSquared = config->m_soulCataclysmSafeZoneRadius * config->m_soulCataclysmSafeZoneRadius;
	const int safeZoneCount = TransformAPI::getChildCount(safeZonesRoot);

	for (int i = 0; i < safeZoneCount; ++i)
	{
		Transform* safeZone = TransformAPI::getChild(safeZonesRoot, i);
		if (!safeZone)
		{
			continue;
		}

		const Vector3 safeZonePosition = TransformAPI::getGlobalPosition(safeZone);
		Vector3 difference = playerPosition - safeZonePosition;
		difference.y = 0.0f;

		if (difference.LengthSquared() <= safeRadiusSquared)
		{
			return true;
		}
	}

	return false;
}

void AelorinSoulCataclysmState::finishCataclysm()
{
	if (m_completed)
	{
		return;
	}

	m_completed = true;

	if(m_lavaController)
	{
		m_lavaController->StartLavaFall(1.456f, 5.0f);
	}

	const bool sent = AnimationAPI::sendTrigger(m_animation, "ToExhaustion");
	if (!sent)
	{
		Debug::warn("[AelorinSoulCataclysmState] Failed to send ToExhaustion trigger");
	}
}

IMPLEMENT_SCRIPT(AelorinSoulCataclysmState)
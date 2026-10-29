#include "pch.h"
#include "LyrielCharacter.h"
#include "ProjectilePool.h"
#include "LyrielDash.h"
#include "LyrielArrowVolley.h"
#include "LyrielSound.h"
#include "PlayerMovement.h"
#include "PlayerAnimationController.h"
#include "LyrielConfig.h"
#include "LyrielBasicAttack.h"
#include "LyrielChargedAttack.h"

#include "PersistingCheckpointState.h"

IMPLEMENT_SCRIPT_FIELDS(LyrielCharacter,
    SERIALIZED_ASSET_REF(m_config, "Lyriel Config", AssetType::DATA_CONTAINER),
    SERIALIZED_STRING(m_arrowSpawnChildName, "Arrow Spawn Child Name")
)

LyrielCharacter::LyrielCharacter(GameObject* owner)
    : CharacterBase(owner)
{
}

void LyrielCharacter::Start()
{
    CharacterBase::Start();

    m_arrowPool     = GameObjectAPI::findScript<ProjectilePool>(getOwner());
    m_basicAttack = GameObjectAPI::findScript<LyrielBasicAttack>(getOwner());
    m_charged       = GameObjectAPI::findScript<LyrielChargedAttack>(getOwner());
    m_chargedAttack = m_charged;
    m_dash          = GameObjectAPI::findScript<LyrielDash>(getOwner());
    m_specialAbility   = GameObjectAPI::findScript<LyrielArrowVolley>(getOwner());
    m_sound         = GameObjectAPI::findScript<LyrielSound>(getOwner());
    m_movement      = GameObjectAPI::findScript<PlayerMovement>(getOwner());
    m_animation     = GameObjectAPI::findScript<PlayerAnimationController>(getOwner());

    if (m_arrowPool == nullptr)
    {
        Debug::log("[LyrielCharacter] ArrowPool not found on owner '%s'.", GameObjectAPI::getName(getOwner()));
    }
	else
	{
		// Lyriel can release five volley arrows in one frame. Build the complete
		// pool, including each arrow's reusable visual variants, before combat.
		m_arrowPool->configurePrewarmToCapacity();
	}

    if (m_basicAttack == nullptr)
    {
        Debug::warn("[LyrielCharacter] LyrielBasicAttack not found on owner '%s'.", GameObjectAPI::getName(getOwner()));
    }

    if (m_chargedAttack == nullptr)
    {
        Debug::warn("[LyrielCharacter] LyrielchargedAttack not found on owner '%s'.", GameObjectAPI::getName(getOwner()));
    }

    if (m_dash == nullptr)
    {
        Debug::log("[LyrielCharacter] LyrielDash not found on owner '%s'.", GameObjectAPI::getName(getOwner()));
    }

    if (m_specialAbility == nullptr)
    {
        Debug::log("[LyrielCharacter] LyrielArrowVolley not found on owner '%s'.", GameObjectAPI::getName(getOwner()));
    }

    if (m_sound == nullptr)
    {
        Debug::log("[LyrielCharacter] LyrielSound not found on owner '%s'.", GameObjectAPI::getName(getOwner()));
    }

    if (m_movement == nullptr)
    {
        Debug::log("[LyrielCharacter] PlayerMovement not found on owner '%s'.", GameObjectAPI::getName(getOwner()));
    }

    if (!PersistingCheckpointState::Get().IsStartOfLevel())
    {
        TransformAPI::setGlobalPosition(GameObjectAPI::getTransform(m_owner),
            PersistingCheckpointState::Get().m_savedLyrielRespawn);
    }
}

void LyrielCharacter::Update()
{
    if (m_sound == nullptr)
    {
        return;
    }

    if (isDowned())
    {
        m_sound->stopAllLoops();
        return;
    }

    if (m_movement != nullptr)
    {
        // Footsteps keep pace with the run clip and stretch out while the bow slows her down.
        const float animSpeed = m_animation != nullptr ? m_animation->getMoveAnimSpeed() : 1.0f;
        const float moveScale = m_movement->getMovementMultiplier();
        if (m_animation != nullptr)
        {
            m_animation->setMoveSpeedScale(moveScale);
        }
        m_sound->setFootstepRate(animSpeed, moveScale);
        m_sound->setFootstepsActive(m_movement->isMoving());
    }
}

void LyrielCharacter::onMarkExploited()
{
    if (m_dash != nullptr)
    {
        m_dash->recoverCharge();
    }

    if (m_specialAbility != nullptr && m_config.get() != nullptr)
    {
        m_specialAbility->reduceCooldown(m_config.get()->m_volleyCooldownReductionPerExploit);
    }
}

IMPLEMENT_SCRIPT(LyrielCharacter)

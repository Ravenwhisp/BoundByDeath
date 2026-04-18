#include "pch.h"
#include "DeathDash.h"
#include "DeathCharacter.h"
#include "PlayerTargetController.h"

static const ScriptFieldInfo DeathDashFields[] =
{
    { "Dash Duration", ScriptFieldType::Float, offsetof(DeathDash, m_dashDuration), { 0.05f, 1.0f, 0.01f } },
};

IMPLEMENT_SCRIPT_FIELDS(DeathDash, DeathDashFields)

DeathDash::DeathDash(GameObject* owner)
    : DeathAbilityBase(owner)
{
    m_cooldown = 0.5f;
}

void DeathDash::Start()
{
    DeathAbilityBase::Start();
}

void DeathDash::Update()
{
    DeathAbilityBase::Update();

    if (m_character == nullptr)
    {
        return;
    }

    // --- Activate dash ---
    if (!m_isDashing && canStartAbility() && Input::isLeftShoulderJustPressed(getPlayerIndex()))
    {
        m_dashTimer = 0.0f;

        // TODO: compute m_dashDirection toward the current target (or character forward if none).
        //       Use m_character->getTargetController()->getCurrentTarget() to get the target,
        //       then derive direction from positions via TransformAPI::getPosition().

        m_isDashing = true;
        setAbilityLocked(true);
        // TODO: play dash animation / start dash VFX
        return;
    }

    // --- Tick dash movement ---
    if (m_isDashing)
    {
        m_dashTimer += Time::getDeltaTime();

        // TODO: move the character along m_dashDirection each frame.
        //       Use: dashDistance = m_deathChar->m_dashDistance
        //       And: TransformAPI::translate(GameObjectAPI::getTransform(getOwner()), ...)

        if (m_dashTimer >= m_dashDuration)
        {
            m_isDashing = false;
            setAbilityLocked(false);
            m_cooldownTimer = m_cooldown;
            // TODO: stop dash VFX
        }
    }
}

IMPLEMENT_SCRIPT(DeathDash)

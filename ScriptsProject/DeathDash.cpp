#include "pch.h"
#include "DeathDash.h"
#include "CharacterBase.h"
#include "DeathCharacter.h"
#include "PlayerTargetController.h"

// ============================================================
// PROPOSAL — This script shows how a timed-movement ability
// should communicate with AbilityBase. All game logic is marked
// TODO and must be properly implemented.
// ============================================================

static const ScriptFieldInfo DeathDashFields[] =
{
    { "Dash Duration", ScriptFieldType::Float, offsetof(DeathDash, m_dashDuration), { 0.05f, 1.0f, 0.01f } },
};

IMPLEMENT_SCRIPT_FIELDS(DeathDash, DeathDashFields)

DeathDash::DeathDash(GameObject* owner)
    : AbilityBase(owner)
{
    m_cooldown = 0.5f;
}

void DeathDash::Start()
{
    m_character = static_cast<CharacterBase*>(
        GameObjectAPI::getScript(m_owner, "DeathCharacter"));

    if (m_character == nullptr)
    {
        Debug::warn("DeathDash: DeathCharacter not found on this GameObject.");
    }
}

void DeathDash::Update()
{
    // MUST be called first — handles dead guard, force-cancel on death, cooldown tick.
    AbilityBase::Update();

    if (m_character == nullptr)
    {
        return;
    }

    // --- Activate dash ---
    if (!isActive() && canActivate() && Input::isLeftShoulderJustPressed(getPlayerIndex()))
    {
        m_dashTimer = 0.0f;

        // TODO: compute m_dashDirection toward the current target (or character forward if none).
        //       Use m_character->getTargetController()->getCurrentTarget() to get the target,
        //       then derive direction from positions via TransformAPI::getPosition().

        onActivate();  // sets isActive = true, blocks other abilities via canAct
        // TODO: play dash animation / start dash VFX
        return;
    }

    // --- Tick dash movement ---
    if (isActive())
    {
        m_dashTimer += Time::getDeltaTime();

        // TODO: move the character along m_dashDirection each frame.
        //       Use: dashDistance = static_cast<DeathCharacter*>(m_character)->m_dashDistance
        //       And: TransformAPI::translate(GameObjectAPI::getTransform(m_owner), ...)

        if (m_dashTimer >= m_dashDuration)
        {
            onDeactivate();  // sets isActive = false, starts cooldown, unblocks canAct
            // TODO: stop dash VFX
        }
    }
}

IMPLEMENT_SCRIPT(DeathDash)

#include "pch.h"
#include "LyrielDash.h"
#include "CharacterBase.h"
#include "LyrielCharacter.h"

// ============================================================
// PROPOSAL — This script shows how a timed-movement ability
// should communicate with AbilityBase. All game logic is marked
// TODO and must be properly implemented.
// ============================================================

static const ScriptFieldInfo LyrielDashFields[] =
{
    { "Dash Duration", ScriptFieldType::Float, offsetof(LyrielDash, m_dashDuration), { 0.05f, 1.0f, 0.01f } },
};

IMPLEMENT_SCRIPT_FIELDS(LyrielDash, LyrielDashFields)

LyrielDash::LyrielDash(GameObject* owner)
    : AbilityBase(owner)
{
    m_cooldown = 0.5f;
}

void LyrielDash::Start()
{
    m_character = static_cast<CharacterBase*>(
        GameObjectAPI::getScript(m_owner, "LyrielCharacter"));

    if (m_character == nullptr)
    {
        Debug::warn("LyrielDash: LyrielCharacter not found on this GameObject.");
    }
}

void LyrielDash::Update()
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

        // TODO: compute m_dashDirection from left stick input (evasive — away from danger).
        //       If stick is neutral, fall back to the character's backward direction.
        //       Use: Input::getMoveAxis(getPlayerIndex()) and TransformAPI::getForward().

        onActivate();  // sets isActive = true, blocks other abilities via canAct
        // TODO: play dodge animation / start evasion VFX
        return;
    }

    // --- Tick dash movement ---
    if (isActive())
    {
        m_dashTimer += Time::getDeltaTime();

        // TODO: move the character along m_dashDirection each frame.
        //       Use: dashDistance = static_cast<LyrielCharacter*>(m_character)->m_dashDistance
        //       And: TransformAPI::translate(GameObjectAPI::getTransform(m_owner), ...)

        if (m_dashTimer >= m_dashDuration)
        {
            onDeactivate();  // sets isActive = false, starts cooldown, unblocks canAct
            // TODO: stop evasion VFX
        }
    }
}

IMPLEMENT_SCRIPT(LyrielDash)

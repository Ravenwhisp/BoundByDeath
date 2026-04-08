#include "pch.h"
#include "LyrielArrowVolley.h"
#include "CharacterBase.h"
#include "LyrielCharacter.h"

// ============================================================
// PROPOSAL — This script shows how an instant multi-hit ability
// should communicate with AbilityBase. All game logic is marked
// TODO and must be properly implemented.
// Mechanic is also pending final design definition.
// ============================================================

static const ScriptFieldInfo LyrielArrowVolleyFields[] =
{
};

IMPLEMENT_SCRIPT_FIELDS(LyrielArrowVolley, LyrielArrowVolleyFields)

LyrielArrowVolley::LyrielArrowVolley(GameObject* owner)
    : AbilityBase(owner)
{
    m_cooldown = 3.0f;
}

void LyrielArrowVolley::Start()
{
    m_character = static_cast<CharacterBase*>(
        GameObjectAPI::getScript(m_owner, "LyrielCharacter"));

    if (m_character == nullptr)
    {
        Debug::warn("LyrielArrowVolley: LyrielCharacter not found on this GameObject.");
    }
}

void LyrielArrowVolley::Update()
{
    // MUST be called first — handles dead guard and cooldown tick.
    AbilityBase::Update();

    if (m_character == nullptr)
    {
        return;
    }

    if (!canActivate() || !Input::isLeftTriggerJustPressed(getPlayerIndex()))
    {
        return;
    }

    // Instant ability: all arrows fire in the same frame.
    onActivate();

    const LyrielCharacter* lyriel = static_cast<LyrielCharacter*>(m_character);
    // TODO: use lyriel->m_arrowVolleyCount and lyriel->m_arrowVolleySpread to spawn
    //       N arrow projectiles spread evenly across the arc centred on the target direction.
    //       Decide whether each arrow hits a different enemy or all hit the same target.
    (void)lyriel;  // suppress unused warning until the above is implemented

    // TODO: play volley animation / spawn projectiles / VFX / sound

    onDeactivate();  // sets isActive = false, starts cooldown, unblocks canAct
}

IMPLEMENT_SCRIPT(LyrielArrowVolley)

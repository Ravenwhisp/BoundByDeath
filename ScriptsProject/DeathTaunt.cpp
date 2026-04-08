#include "pch.h"
#include "DeathTaunt.h"
#include "CharacterBase.h"
#include "DeathCharacter.h"

// ============================================================
// PROPOSAL — This script shows how a timed passive ability
// should communicate with AbilityBase. All game logic is marked
// TODO and must be properly implemented.
// Mechanic is also pending final design definition.
// ============================================================

static const ScriptFieldInfo DeathTauntFields[] =
{
};

IMPLEMENT_SCRIPT_FIELDS(DeathTaunt, DeathTauntFields)

DeathTaunt::DeathTaunt(GameObject* owner)
    : AbilityBase(owner)
{
}

void DeathTaunt::Start()
{
    m_character = static_cast<CharacterBase*>(
        GameObjectAPI::getScript(m_owner, "DeathCharacter"));

    if (m_character == nullptr)
    {
        Debug::warn("DeathTaunt: DeathCharacter not found on this GameObject.");
    }
}

void DeathTaunt::Update()
{
    // MUST be called first — handles dead guard, force-cancel on death, cooldown tick.
    AbilityBase::Update();

    if (m_character == nullptr)
    {
        return;
    }

    // --- Activate taunt ---
    if (!isActive() && canActivate() && Input::isLeftTriggerJustPressed(getPlayerIndex()))
    {
        m_tauntTimer = 0.0f;
        onActivate();  // sets isActive = true, blocks other abilities via canAct
        // TODO: play taunt animation / VFX
        // TODO: apply taunt effect to nearby enemies (aggro, debuff, etc.) — pending design
        return;
    }

    // --- Tick taunt duration ---
    if (isActive())
    {
        m_tauntTimer += Time::getDeltaTime();

        const float tauntDuration = static_cast<DeathCharacter*>(m_character)->m_tauntDuration;

        // TODO: sustain taunt effect on enemies each frame

        if (m_tauntTimer >= tauntDuration)
        {
            // TODO: remove taunt effect from enemies
            onDeactivate();  // sets isActive = false, starts cooldown, unblocks canAct
        }
    }
}

IMPLEMENT_SCRIPT(DeathTaunt)

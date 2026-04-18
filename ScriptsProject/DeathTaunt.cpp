#include "pch.h"
#include "DeathTaunt.h"
#include "DeathCharacter.h"

// TODO: define taunt mechanic — pending design definition.

ScriptFieldList DeathTaunt::getExposedFields() const { return {}; }

DeathTaunt::DeathTaunt(GameObject* owner)
    : DeathAbilityBase(owner)
{
}

void DeathTaunt::Start()
{
    DeathAbilityBase::Start();
}

void DeathTaunt::Update()
{
    DeathAbilityBase::Update();

    if (m_character == nullptr)
    {
        return;
    }

    // --- Activate taunt ---
    if (!m_isTaunting && canStartAbility() && Input::isLeftTriggerJustPressed(getPlayerIndex()))
    {
        m_tauntTimer = 0.0f;
        m_isTaunting = true;
        setAbilityLocked(true);
        // TODO: play taunt animation / VFX
        // TODO: apply taunt effect to nearby enemies — pending design
        return;
    }

    // --- Tick taunt duration ---
    if (m_isTaunting)
    {
        m_tauntTimer += Time::getDeltaTime();

        // TODO: sustain taunt effect on enemies each frame

        if (m_tauntTimer >= m_deathChar->m_tauntDuration)
        {
            // TODO: remove taunt effect from enemies
            m_isTaunting = false;
            setAbilityLocked(false);
            m_cooldownTimer = m_cooldown;
        }
    }
}

IMPLEMENT_SCRIPT(DeathTaunt)

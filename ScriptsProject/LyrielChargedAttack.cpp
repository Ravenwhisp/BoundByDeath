#include "pch.h"
#include "LyrielChargedAttack.h"
#include "CharacterBase.h"
#include "LyrielCharacter.h"

// ============================================================
// PROPOSAL — This script shows how a hold-to-aim ability
// should communicate with AbilityBase. All game logic is marked
// TODO and must be properly implemented.
// ============================================================

/*static const ScriptFieldInfo LyrielChargedAttackFields[] =
{
};

IMPLEMENT_SCRIPT_FIELDS(LyrielChargedAttack, LyrielChargedAttackFields)*/

LyrielChargedAttack::LyrielChargedAttack(GameObject* owner)
    : AbilityBase(owner)
{
}

void LyrielChargedAttack::Start()
{
    m_character = static_cast<CharacterBase*>(
        GameObjectAPI::getScript(m_owner, "LyrielCharacter"));

    if (m_character == nullptr)
    {
        Debug::warn("LyrielChargedAttack: LyrielCharacter not found on this GameObject.");
    }
}

void LyrielChargedAttack::Update()
{
    // MUST be called first — handles dead guard, force-cancel on death, cooldown tick.
    AbilityBase::Update();

    if (m_character == nullptr)
    {
        return;
    }

    // --- Enter aiming mode (button pressed) ---
    // onActivate blocks all other abilities for the full aiming duration.
    if (!isActive() && canActivate() && Input::isRightTriggerJustPressed(getPlayerIndex()))
    {
        m_aimDirection = { 0.0f, 0.0f };
        onActivate();  // sets isActive = true, blocks other abilities via canAct
        // TODO: play draw-bow animation / show aiming reticle
        // TODO: optionally slow down PlayerController movement speed while aiming
        return;
    }

    // --- While aiming (button held) ---
    if (isActive())
    {
        // Update aim direction from the right stick each frame.
        m_aimDirection = Input::getLookAxis(getPlayerIndex());
        // TODO: rotate Lyriel or update a directional indicator based on m_aimDirection

        // --- Fire on release ---
        if (Input::isRightTriggerReleased(getPlayerIndex()))
        {
            const float damage = static_cast<LyrielCharacter*>(m_character)->m_chargedArrowDamage;

            // TODO: spawn charged arrow projectile in m_aimDirection (world space)
            // Placeholder: deal damage directly to the current target.
            dealDamageToCurrentTarget(damage);

            // TODO: play release animation / impact VFX / sound
            // TODO: restore PlayerController speed if it was reduced during aiming

            onDeactivate();  // sets isActive = false, starts cooldown, unblocks canAct
        }
    }
}

IMPLEMENT_SCRIPT(LyrielChargedAttack)

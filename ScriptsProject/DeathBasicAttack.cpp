#include "pch.h"
#include "DeathBasicAttack.h"
#include "CharacterBase.h"
#include "DeathCharacter.h"

// ============================================================
// PROPOSAL — This script shows how a basic combo attack should
// communicate with AbilityBase. All game logic is marked TODO
// and must be properly implemented.
// ============================================================

static const ScriptFieldInfo DeathBasicAttackFields[] =
{
    { "Combo Window", ScriptFieldType::Float, offsetof(DeathBasicAttack, m_comboWindow), { 0.1f, 3.0f, 0.05f } },
};

IMPLEMENT_SCRIPT_FIELDS(DeathBasicAttack, DeathBasicAttackFields)

DeathBasicAttack::DeathBasicAttack(GameObject* owner)
    : AbilityBase(owner)
{
}

void DeathBasicAttack::Start()
{
    // Get the sibling CharacterBase (DeathCharacter) from the same GameObject.
    // Cast to CharacterBase* to use the shared interface.
    // Cast to DeathCharacter* when Death-specific stats are needed (see Update).
    m_character = static_cast<CharacterBase*>(
        GameObjectAPI::getScript(m_owner, "DeathCharacter"));

    if (m_character == nullptr)
    {
        Debug::warn("DeathBasicAttack: DeathCharacter not found on this GameObject.");
    }
}

void DeathBasicAttack::Update()
{
    // MUST be called first — handles dead guard, force-cancel on death, cooldown tick.
    AbilityBase::Update();

    if (m_character == nullptr)
    {
        return;
    }

    // --- Combo timeout ---
    // TODO: advance m_comboTimer each frame; if it exceeds m_comboWindow, reset m_comboStep to 0.

    // --- Input + activation check ---
    // canActivate() covers: isEnabled, !isActive, character alive, canAct, cooldown ready.
    if (!canActivate() || !Input::isRightShoulderJustPressed(getPlayerIndex()))
    {
        return;
    }

    // Each hit is instant: activate → apply effect → deactivate in the same frame.
    // This keeps canAct free between hits so the player can dash out of a combo.
    onActivate();  // sets isActive = true, blocks other abilities via canAct

    // Read Death's damage stat through the concrete type.
    const float damage = static_cast<DeathCharacter*>(m_character)->m_basicAttackDamage;

    // TODO: advance m_comboStep (1 → 2 → 3 → reset to 0)
    // TODO: optionally scale damage per combo step
    // TODO: play the corresponding hit animation for the current m_comboStep
    dealDamageToCurrentTarget(damage);

    onDeactivate();  // sets isActive = false, starts cooldown, unblocks canAct
}

IMPLEMENT_SCRIPT(DeathBasicAttack)

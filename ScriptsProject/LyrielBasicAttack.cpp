#include "pch.h"
#include "LyrielBasicAttack.h"
#include "CharacterBase.h"
#include "LyrielCharacter.h"

// ============================================================
// PROPOSAL — This script shows how a simple instant-fire ability
// should communicate with AbilityBase. All game logic is marked
// TODO and must be properly implemented.
// ============================================================

/*static const ScriptFieldInfo LyrielBasicAttackFields[] =
{
};

IMPLEMENT_SCRIPT_FIELDS(LyrielBasicAttack, LyrielBasicAttackFields)*/

LyrielBasicAttack::LyrielBasicAttack(GameObject* owner)
    : AbilityBase(owner)
{
    m_cooldown = 0.4f;
}

void LyrielBasicAttack::Start()
{
    m_character = static_cast<CharacterBase*>(
        GameObjectAPI::getScript(m_owner, "LyrielCharacter"));

    if (m_character == nullptr)
    {
        Debug::warn("LyrielBasicAttack: LyrielCharacter not found on this GameObject.");
    }
}

void LyrielBasicAttack::Update()
{
    // MUST be called first — handles dead guard and cooldown tick.
    AbilityBase::Update();

    if (m_character == nullptr)
    {
        return;
    }

    // canActivate() covers: isEnabled, !isActive, character alive, canAct, cooldown ready.
    if (!canActivate() || !Input::isRightShoulderJustPressed(getPlayerIndex()))
    {
        return;
    }

    // Instant ability: activate → apply effect → deactivate in the same frame.
    onActivate();

    const float damage = static_cast<LyrielCharacter*>(m_character)->m_basicArrowDamage;
    dealDamageToCurrentTarget(damage);

    // TODO: spawn arrow projectile travelling toward the current target
    // TODO: play shoot animation / VFX / sound

    onDeactivate();  // sets isActive = false, starts cooldown, unblocks canAct
}

IMPLEMENT_SCRIPT(LyrielBasicAttack)

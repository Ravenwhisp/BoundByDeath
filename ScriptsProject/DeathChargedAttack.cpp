#include "pch.h"
#include "DeathChargedAttack.h"
#include "CharacterBase.h"
#include "DeathCharacter.h"

// ============================================================
// PROPOSAL — This script shows how a hold-to-charge ability
// should communicate with AbilityBase. All game logic is marked
// TODO and must be properly implemented.
// ============================================================

static const ScriptFieldInfo DeathChargedAttackFields[] =
{
    { "Min Charge Time", ScriptFieldType::Float, offsetof(DeathChargedAttack, m_minChargeTime), { 0.0f, 3.0f, 0.05f } },
};

IMPLEMENT_SCRIPT_FIELDS(DeathChargedAttack, DeathChargedAttackFields)

DeathChargedAttack::DeathChargedAttack(GameObject* owner)
    : AbilityBase(owner)
{
}

void DeathChargedAttack::Start()
{
    m_character = static_cast<CharacterBase*>(
        GameObjectAPI::getScript(m_owner, "DeathCharacter"));

    if (m_character == nullptr)
    {
        Debug::warn("DeathChargedAttack: DeathCharacter not found on this GameObject.");
    }
}

void DeathChargedAttack::Update()
{
    // MUST be called first — handles dead guard, force-cancel on death, cooldown tick.
    AbilityBase::Update();

    if (m_character == nullptr)
    {
        return;
    }

    // --- Start charging (button pressed) ---
    // onActivate blocks all other abilities for the entire charge duration.
    if (!isActive() && canActivate() && Input::isRightTriggerJustPressed(getPlayerIndex()))
    {
        m_chargeTime = 0.0f;
        onActivate();  // sets isActive = true, blocks other abilities via canAct
        // TODO: play charge-up animation / start charge VFX
        return;
    }

    // --- While charging (button held) ---
    if (isActive())
    {
        m_chargeTime += Time::getDeltaTime();
        // TODO: update charge VFX intensity proportional to m_chargeTime / m_minChargeTime

        // --- Release (button released) ---
        if (Input::isRightTriggerReleased(getPlayerIndex()))
        {
            if (m_chargeTime >= m_minChargeTime)
            {
                // Fully charged — deal heavy damage.
                const float damage = static_cast<DeathCharacter*>(m_character)->m_chargedAttackDamage;
                dealDamageToCurrentTarget(damage);
                // TODO: play heavy strike animation / impact VFX / sound
            }
            // else: released too early — attack fizzles, no effect.

            m_chargeTime = 0.0f;
            onDeactivate();  // sets isActive = false, starts cooldown, unblocks canAct
        }
    }
}

IMPLEMENT_SCRIPT(DeathChargedAttack)

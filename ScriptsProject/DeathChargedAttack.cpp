#include "pch.h"
#include "DeathChargedAttack.h"
#include "CharacterBase.h"
#include "DeathCharacter.h"

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
    // MUST be called first — dead guard + cooldown tick.
    AbilityBase::Update();

    if (m_character == nullptr)
    {
        return;
    }

    DeathCharacter* deathChar = static_cast<DeathCharacter*>(m_character);

    // --- Start charging (R2 just pressed) ---
    if (Input::isRightTriggerJustPressed(getPlayerIndex()))
    {
        if (!canActivate())
        {
            if (m_character->isDead())
                Debug::log("[R2] bloqueado — personaje muerto");
            else if (!m_character->canAct())
                Debug::log("[R2] bloqueado — otra ability activa");
            else
                Debug::log("[R2] bloqueado — cooldown o desactivado");
        }
        else if (!deathChar->canUseR2InCombo())
        {
            Debug::log("[R2] bloqueado — 2 R2 consecutivos ya usados, usa R1 primero");
        }
        else
        {
            m_chargeTime = 0.0f;
            onActivate();
            Debug::log("[COMBO] R2 cargando  step=%d/3", deathChar->getComboStep() + 1);
            return;
        }
        return;
    }

    // --- While charging (trigger held) ---
    if (!isActive())
    {
        return;
    }

    m_chargeTime += Time::getDeltaTime();
    // TODO: update charge VFX intensity proportional to m_chargeTime / deathChar->m_maxChargeTime

    // --- Release (trigger released) ---
    if (!Input::isRightTriggerReleased(getPlayerIndex()))
    {
        return;
    }

    // A "charged" release requires:
    //   - held long enough (>= m_minChargeTime), AND
    //   - started with no active combo (combo starter rule).
    // If either condition fails the hit is treated as a quick heavy.
    const bool isChargedRelease = (m_chargeTime >= m_minChargeTime)
                                  && (deathChar->getComboStep() == 0);

    const int  stepBefore = deathChar->getComboStep();
    const bool isLast     = (stepBefore >= 2);

    if (isChargedRelease)
    {
        const float rawRatio    = m_chargeTime / deathChar->m_maxChargeTime;
        const float chargeRatio = rawRatio > 1.0f ? 1.0f : rawRatio;
        const float damage      = deathChar->m_chargedAttackDamage * (1.0f + chargeRatio);
        const float stunDuration = deathChar->m_briefStunDuration
            + chargeRatio * (deathChar->m_extendedStunDuration - deathChar->m_briefStunDuration);

        if (isLast)
            Debug::log("[COMBO] R2 CARGADO  step 3/3  COMPLETO — reset  ratio=%.0f%%  dmg=%.1f  stun=%.2fs",
                chargeRatio * 100.0f, damage, stunDuration);
        else
            Debug::log("[COMBO] R2 CARGADO  step %d/3  ventana=%.1fs  ratio=%.0f%%  dmg=%.1f  stun=%.2fs",
                stepBefore + 1, deathChar->m_comboWindow, chargeRatio * 100.0f, damage, stunDuration);

        deathChar->dealDamageInArc(damage, stunDuration);
    }
    else
    {
        if (isLast)
            Debug::log("[COMBO] R2  step 3/3  COMPLETO — reset  dmg=%.1f  stun=%.2fs",
                deathChar->m_chargedAttackDamage, deathChar->m_briefStunDuration);
        else
            Debug::log("[COMBO] R2  step %d/3  ventana=%.1fs  dmg=%.1f  stun=%.2fs",
                stepBefore + 1, deathChar->m_comboWindow,
                deathChar->m_chargedAttackDamage, deathChar->m_briefStunDuration);

        deathChar->dealDamageInArc(deathChar->m_chargedAttackDamage,
                                   deathChar->m_briefStunDuration);
    }

    deathChar->advanceCombo(true);

    m_chargeTime = 0.0f;
    onDeactivate();
}

IMPLEMENT_SCRIPT(DeathChargedAttack)

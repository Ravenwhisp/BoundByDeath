#include "pch.h"
#include "DeathChargedAttack.h"
#include "DeathCharacter.h"

static const ScriptFieldInfo DeathChargedAttackFields[] =
{
    { "Min Charge Time", ScriptFieldType::Float, offsetof(DeathChargedAttack, m_minChargeTime), { 0.0f, 3.0f, 0.05f } },
};

IMPLEMENT_SCRIPT_FIELDS(DeathChargedAttack, DeathChargedAttackFields)

DeathChargedAttack::DeathChargedAttack(GameObject* owner)
    : DeathAbilityBase(owner)
{
}

void DeathChargedAttack::Start()
{
    DeathAbilityBase::Start();
}

void DeathChargedAttack::Update()
{
    DeathAbilityBase::Update();

    if (m_character == nullptr)
    {
        return;
    }

    // --- Start charging (R2 just pressed) ---
    if (Input::isRightTriggerJustPressed(getPlayerIndex()))
    {
        if (m_deathChar->isInComboCooldown())
        {
            Debug::log("[R2] bloqueado — cooldown post-combo");
        }
        else if (!canStartAbility())
        {
            if (m_character->isDowned())
                Debug::log("[R2] bloqueado — personaje caido");
            else if (m_character->isUsingAbility())
                Debug::log("[R2] bloqueado — otra ability activa");
            else
                Debug::log("[R2] bloqueado — cooldown o desactivado");
        }
        else if (!m_deathChar->canUseR2InCombo())
        {
            Debug::log("[R2] bloqueado — 2 R2 consecutivos ya usados, usa R1 primero");
        }
        else
        {
            m_chargeTime = 0.0f;
            m_isCharging = true;
            setAbilityLocked(true);
            Debug::log("[COMBO] R2 cargando  step=%d/3", m_deathChar->getComboStep() + 1);
            return;
        }
        return;
    }

    // --- While charging (trigger held) ---
    if (!m_isCharging)
    {
        return;
    }

    m_chargeTime += Time::getDeltaTime();
    // TODO: update charge VFX intensity proportional to m_chargeTime / m_deathChar->m_maxChargeTime

    // --- Release (trigger released) ---
    if (!Input::isRightTriggerReleased(getPlayerIndex()))
    {
        return;
    }

    const bool isChargedRelease = (m_chargeTime >= m_minChargeTime)
                                  && (m_deathChar->getComboStep() == 0);

    const int  stepBefore = m_deathChar->getComboStep();
    const bool isLast     = (stepBefore >= 2);

    if (isChargedRelease)
    {
        const float rawRatio    = m_chargeTime / m_deathChar->m_maxChargeTime;
        const float chargeRatio = rawRatio > 1.0f ? 1.0f : rawRatio;
        const float damage      = m_deathChar->m_chargedAttackDamage * (1.0f + chargeRatio);

        if (isLast)
        {
            Debug::log("[COMBO] R2 CARGADO  step 3/3  COMPLETO — reset  ratio=%.0f%%  dmg=%.1f",
                chargeRatio * 100.0f, damage);
        }
        else
        {
            Debug::log("[COMBO] R2 CARGADO  step %d/3  ventana=%.1fs  ratio=%.0f%%  dmg=%.1f",
                stepBefore + 1, m_deathChar->m_comboWindow, chargeRatio * 100.0f, damage);
        }

        m_deathChar->dealDamageInArc(damage);
    }
    else
    {
        if (isLast)
        {
            Debug::log("[COMBO] R2  step 3/3  COMPLETO — reset  dmg=%.1f",
                m_deathChar->m_chargedAttackDamage);
        }
        else
        {
            Debug::log("[COMBO] R2  step %d/3  ventana=%.1fs  dmg=%.1f",
                stepBefore + 1, m_deathChar->m_comboWindow, m_deathChar->m_chargedAttackDamage);
        }

        m_deathChar->dealDamageInArc(m_deathChar->m_chargedAttackDamage);
    }

    m_deathChar->advanceCombo(true);

    m_chargeTime = 0.0f;
    m_isCharging = false;
    setAbilityLocked(false);
}

IMPLEMENT_SCRIPT(DeathChargedAttack)

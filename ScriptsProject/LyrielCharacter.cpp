#include "pch.h"
#include "LyrielCharacter.h"

static const ScriptFieldInfo LyrielCharacterFields[] =
{
    // Inherited from Damageable
    { "Max HP",               ScriptFieldType::Float, offsetof(LyrielCharacter, m_maxHp),              { 0.0f, 999.0f, 1.0f } },
    // Inherited from CharacterBase
    { "Player Index",         ScriptFieldType::Int,   offsetof(LyrielCharacter, m_playerIndex) },
    // Lyriel-specific combat stats — read by sibling ability scripts
    { "Basic Arrow Damage",   ScriptFieldType::Float, offsetof(LyrielCharacter, m_basicArrowDamage),   { 0.0f, 200.0f, 1.0f } },
    { "Charged Arrow Damage", ScriptFieldType::Float, offsetof(LyrielCharacter, m_chargedArrowDamage), { 0.0f, 200.0f, 1.0f } },
    { "Dash Distance",        ScriptFieldType::Float, offsetof(LyrielCharacter, m_dashDistance),       { 0.0f, 20.0f,  0.1f } },
    { "Arrow Volley Count",   ScriptFieldType::Int,   offsetof(LyrielCharacter, m_arrowVolleyCount) },
    { "Arrow Volley Spread",  ScriptFieldType::Float, offsetof(LyrielCharacter, m_arrowVolleySpread),  { 0.0f, 180.0f, 1.0f } },
};

IMPLEMENT_SCRIPT_FIELDS(LyrielCharacter, LyrielCharacterFields)

LyrielCharacter::LyrielCharacter(GameObject* owner)
    : CharacterBase(owner)
{
}

void LyrielCharacter::Start()
{
    CharacterBase::Start();
}

void LyrielCharacter::onDamaged(float amount)
{
    CharacterBase::onDamaged(amount);
    // TODO: Lyriel-specific hit reaction (animation, VFX, sound)
}

void LyrielCharacter::onDeath()
{
    CharacterBase::onDeath();  // disables canAct + PlayerController
    // TODO: Lyriel-specific death response (animation, sound)
    Debug::log("Lyriel has died.");
}

void LyrielCharacter::onRevive()
{
    CharacterBase::onRevive();  // restores canAct + PlayerController
    // TODO: Lyriel-specific revive response (animation, sound)
    Debug::log("Lyriel has revived.");
}

IMPLEMENT_SCRIPT(LyrielCharacter)

#include "pch.h"
#include "DeathCharacter.h"

static const ScriptFieldInfo DeathCharacterFields[] =
{
    // Inherited from Damageable
    { "Max HP",                ScriptFieldType::Float, offsetof(DeathCharacter, m_maxHp),               { 0.0f, 999.0f, 1.0f } },
    // Inherited from CharacterBase
    { "Player Index",          ScriptFieldType::Int,   offsetof(DeathCharacter, m_playerIndex) },
    // Death-specific combat stats — read by sibling ability scripts
    { "Basic Attack Damage",   ScriptFieldType::Float, offsetof(DeathCharacter, m_basicAttackDamage),   { 0.0f, 200.0f, 1.0f } },
    { "Charged Attack Damage", ScriptFieldType::Float, offsetof(DeathCharacter, m_chargedAttackDamage), { 0.0f, 200.0f, 1.0f } },
    { "Dash Distance",         ScriptFieldType::Float, offsetof(DeathCharacter, m_dashDistance),        { 0.0f, 20.0f,  0.1f } },
    { "Taunt Duration",        ScriptFieldType::Float, offsetof(DeathCharacter, m_tauntDuration),       { 0.0f, 10.0f,  0.1f } },
};

IMPLEMENT_SCRIPT_FIELDS(DeathCharacter, DeathCharacterFields)

DeathCharacter::DeathCharacter(GameObject* owner)
    : CharacterBase(owner)
{
}

void DeathCharacter::Start()
{
    CharacterBase::Start();
}

void DeathCharacter::onDamaged(float amount)
{
    CharacterBase::onDamaged(amount);
    // TODO: Death-specific hit reaction (animation, VFX, sound)
}

void DeathCharacter::onDeath()
{
    CharacterBase::onDeath();  // disables canAct + PlayerController
    // TODO: Death-specific death response (animation, sound)
    Debug::log("Death has died.");
}

void DeathCharacter::onRevive()
{
    CharacterBase::onRevive();  // restores canAct + PlayerController
    // TODO: Death-specific revive response (animation, sound)
    Debug::log("Death has revived.");
}

IMPLEMENT_SCRIPT(DeathCharacter)

#include "pch.h"
#include "DebugDamageTrigger.h"
#include "Damageable.h"

static const ScriptFieldInfo debugDamageTriggerFields[] =
{
    { "Damage Amount", ScriptFieldType::Float, offsetof(DebugDamageTrigger, m_damageAmount), { 0.0f, 999999.0f, 1.0f } },
    { "Heal Amount", ScriptFieldType::Float, offsetof(DebugDamageTrigger, m_healAmount), { 0.0f, 999999.0f, 1.0f } }
};

IMPLEMENT_SCRIPT_FIELDS(DebugDamageTrigger, debugDamageTriggerFields)

DebugDamageTrigger::DebugDamageTrigger(GameObject* owner)
    : Script(owner)
{
}

void DebugDamageTrigger::Start()
{
    m_damageable = findDamageable();

    if (!m_damageable)
    {
        Debug::warn("DebugDamageTrigger on '%s' could not find a Damageable or PlayerDamageable on the same GameObject.",  GameObjectAPI::getName(m_owner));
    }
}

void DebugDamageTrigger::Update()
{
    if (!m_damageable)
    {
        return;
    }

    if (Input::isKeyDown(KeyCode::J))
    {
        Debug::log("DebugDamageTrigger: damaging '%s' by %.2f", GameObjectAPI::getName(m_owner), m_damageAmount);

        m_damageable->takeDamage(m_damageAmount);
    }

    if (Input::isKeyDown(KeyCode::K))
    {
        Debug::log("DebugDamageTrigger: healing '%s' by %.2f", GameObjectAPI::getName(m_owner), m_healAmount);

        m_damageable->heal(m_healAmount);
    }

    if (Input::isKeyDown(KeyCode::L))
    {
        Debug::log("DebugDamageTrigger: killing '%s'", GameObjectAPI::getName(m_owner));

        m_damageable->kill();
    }

    if (Input::isKeyDown(KeyCode::R))
    {
        Debug::log("DebugDamageTrigger: reviving '%s'", GameObjectAPI::getName(m_owner));

        m_damageable->revive();
    }
}

Damageable* DebugDamageTrigger::findDamageable() const
{
    if (!m_owner)
    {
        return nullptr;
    }

    Script* script = GameObjectAPI::getScriptByName(m_owner, "PlayerDamageable");
    Damageable* damageable = dynamic_cast<Damageable*>(script);

    if (damageable)
    {
        return damageable;
    }

    script = GameObjectAPI::getScriptByName(m_owner, "Damageable");
    damageable = dynamic_cast<Damageable*>(script);

    return damageable;
}

IMPLEMENT_SCRIPT(DebugDamageTrigger)
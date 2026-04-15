#include "pch.h"
#include "Damageable.h"

static const ScriptFieldInfo damageableFields[] =
{
    { "Max HP", ScriptFieldType::Float, offsetof(Damageable, m_maxHp), { 0.0f, 999999.0f, 1.0f } },
};

IMPLEMENT_SCRIPT_FIELDS(Damageable, damageableFields)

Damageable::Damageable(GameObject* owner)
    : Script(owner)
{
}

void Damageable::Start()
{
    m_currentHp = m_maxHp;
    clampHp();
    m_isDead = (m_currentHp <= 0.0f);
}

void Damageable::takeDamage(float amount)
{
    if (m_isDead)
    {
        return;
    }

    if (m_invulnerable)
    {
        return;
    }

    if (amount <= 0.0f)
    {
        return;
    }

    m_currentHp -= amount;
    clampHp();

    onDamaged(amount);

    if (m_currentHp <= 0.0f)
    {
        onHpDepleted();
    }
}

void Damageable::heal(float amount)
{
    if (m_isDead)
    {
        return;
    }

    if (amount <= 0.0f)
    {
        return;
    }

    m_currentHp += amount;
    clampHp();

    onHealed(amount);
}

void Damageable::kill()
{
    if (m_isDead)
    {
        return;
    }

    m_currentHp = 0.0f;
    m_isDead = true;

    onDeath();
}

void Damageable::revive(float hp)
{
    m_isDead = false;

    if (hp < 0.0f)
    {
        m_currentHp = m_maxHp;
    }
    else
    {
        m_currentHp = hp;
        clampHp();

        if (m_currentHp <= 0.0f && m_maxHp > 0.0f)
        {
            m_currentHp = m_maxHp;
        }
    }

    clampHp();
    onRevive();
}

float Damageable::getHpPercent() const
{
    if (m_maxHp <= 0.0f)
    {
        return 0.0f;
    }

    return m_currentHp / m_maxHp;
}

void Damageable::clampHp()
{
    if (m_maxHp < 0.0f)
    {
        m_maxHp = 0.0f;
    }

    if (m_currentHp < 0.0f)
    {
        m_currentHp = 0.0f;
    }

    if (m_currentHp > m_maxHp)
    {
        m_currentHp = m_maxHp;
    }
}

void Damageable::onDamaged(float amount)
{
    Debug::log("%s took %.2f damage. HP: %.2f / %.2f", GameObjectAPI::getName(m_owner), amount, m_currentHp, m_maxHp);
}

void Damageable::onHealed(float amount)
{
    Debug::log("%s healed %.2f HP. HP: %.2f / %.2f", GameObjectAPI::getName(m_owner), amount, m_currentHp, m_maxHp);
}

void Damageable::onHpDepleted()
{
    kill();
}

void Damageable::onDeath()
{
    Debug::log("%s died.", GameObjectAPI::getName(m_owner));
}

void Damageable::onRevive()
{
    Debug::log("%s revived. HP: %.2f / %.2f", GameObjectAPI::getName(m_owner), m_currentHp, m_maxHp);
}

void Damageable::drawGizmo()
{
    if (!m_isStunned)
    {
        return;
    }

    const Transform* t = GameObjectAPI::getTransform(m_owner);
    if (t == nullptr)
    {
        return;
    }

    const Vector3 pos    = TransformAPI::getPosition(t);
    const Vector3 center = { pos.x, pos.y + 2.2f, pos.z };
    const Vector3 colYellow = { 1.0f, 0.9f, 0.0f };

    // Outer ring
    DebugDrawAPI::drawCircle(center, { 0.0f, 1.0f, 0.0f }, colYellow, 0.4f, 12.0f);

    // Four short radiating spikes around the ring — classic stun stars
    constexpr float k_pi     = 3.14159265f;
    constexpr float spikeLen = 0.25f;
    for (int i = 0; i < 4; ++i)
    {
        const float angle = (k_pi * 0.5f) * static_cast<float>(i);
        const Vector3 dir  = { cosf(angle), 0.0f, sinf(angle) };
        const Vector3 base = center + dir * 0.4f;
        DebugDrawAPI::drawLine(base, base + dir * spikeLen, colYellow);
    }

    // Inner dot
    DebugDrawAPI::drawPoint(center, colYellow, 4.0f);
}

void Damageable::Update()
{
    if (m_isStunned)
    {
        m_stunTimer -= Time::getDeltaTime();
        if (m_stunTimer <= 0.0f)
        {
            m_stunTimer = 0.0f;
            m_isStunned = false;
        }
    }
}

void Damageable::stun(float duration)
{
    if (m_isDead || duration <= 0.0f)
    {
        return;
    }

    // Only extend the timer, never reduce it.
    if (duration > m_stunTimer)
    {
        m_stunTimer = duration;
    }
    m_isStunned = true;
}

void Damageable::clearStun()
{
    m_stunTimer = 0.0f;
    m_isStunned = false;
}

IMPLEMENT_SCRIPT(Damageable)
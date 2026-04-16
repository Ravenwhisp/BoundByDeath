#include "pch.h"
#include "DeathCharacter.h"
#include "Damageable.h"

#include <cmath>
#include <vector>

static const ScriptFieldInfo DeathCharacterFields[] =
{
    { "Max HP",                ScriptFieldType::Float, offsetof(DeathCharacter, m_maxHp),               { 0.0f,  999.0f, 1.0f  } },
    { "Player Index",          ScriptFieldType::Int,   offsetof(DeathCharacter, m_playerIndex) },
    { "Basic Attack Damage",   ScriptFieldType::Float, offsetof(DeathCharacter, m_basicAttackDamage),   { 0.0f,  200.0f, 1.0f  } },
    { "Charged Attack Damage", ScriptFieldType::Float, offsetof(DeathCharacter, m_chargedAttackDamage), { 0.0f,  200.0f, 1.0f  } },
    { "Dash Distance",         ScriptFieldType::Float, offsetof(DeathCharacter, m_dashDistance),        { 0.0f,  20.0f,  0.1f  } },
    { "Taunt Duration",        ScriptFieldType::Float, offsetof(DeathCharacter, m_tauntDuration),       { 0.0f,  10.0f,  0.1f  } },
    { "Arc Range",             ScriptFieldType::Float, offsetof(DeathCharacter, m_arcRange),            { 0.5f,  10.0f,  0.1f  } },
    { "Arc Angle",             ScriptFieldType::Float, offsetof(DeathCharacter, m_arcAngle),            { 10.0f, 360.0f, 5.0f  } },
    { "Max Charge Time",       ScriptFieldType::Float, offsetof(DeathCharacter, m_maxChargeTime),       { 0.5f,  5.0f,   0.1f  } },
    { "Combo Window",          ScriptFieldType::Float, offsetof(DeathCharacter, m_comboWindow),         { 0.1f,  3.0f,   0.05f } },
    { "Combo Cooldown",        ScriptFieldType::Float, offsetof(DeathCharacter, m_comboCooldown),       { 0.0f,  5.0f,   0.1f  } },
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

void DeathCharacter::Update()
{
    CharacterBase::Update();
    tickCombo(Time::getDeltaTime());
}

void DeathCharacter::tickCombo(float dt)
{
    if (m_comboCooldownTimer > 0.0f)
    {
        m_comboCooldownTimer -= dt;
    }

    if (m_comboStep == 0)
    {
        return;
    }

    m_comboTimer += dt;
    if (m_comboTimer >= m_comboWindow)
    {
        resetCombo();
    }
}

void DeathCharacter::advanceCombo(bool isR2)
{
    m_comboTimer = 0.0f;

    if (isR2)
    {
        m_consecutiveR2Count++;
    }
    else
    {
        m_consecutiveR2Count = 0;
    }

    m_comboStep++;

    if (m_comboStep >= 3)
    {
        resetCombo();
        m_comboCooldownTimer = m_comboCooldown;
    }
}

void DeathCharacter::resetCombo()
{
    m_comboStep          = 0;
    m_consecutiveR2Count = 0;
    m_comboTimer         = 0.0f;
}

void DeathCharacter::dealDamageInArc(float damage) const
{
    const Transform* myTransform = GameObjectAPI::getTransform(m_owner);
    if (myTransform == nullptr)
    {
        return;
    }

    Vector3 myPos     = TransformAPI::getPosition(myTransform);
    Vector3 myForward = TransformAPI::getForward(myTransform);

    myForward.y = 0.0f;
    const float fwdLen = myForward.Length();
    if (fwdLen > 0.0001f)
    {
        myForward /= fwdLen;
    }

    constexpr float k_degToRad   = 3.14159265f / 180.0f;
    const float     halfAngleCos = cosf(m_arcAngle * 0.5f * k_degToRad);
    const float     arcRangeSq   = m_arcRange * m_arcRange;

    const auto enemies = SceneAPI::findAllGameObjectsByTag(Tag::ENEMY);
    int scanned = 0;
    int hit     = 0;

    for (GameObject* enemy : enemies)
    {
        if (enemy == nullptr)
        {
            continue;
        }

        const Transform* enemyTr = GameObjectAPI::getTransform(enemy);
        if (enemyTr == nullptr)
        {
            continue;
        }

        scanned++;

        Vector3 toEnemy = TransformAPI::getPosition(enemyTr) - myPos;
        toEnemy.y = 0.0f;

        const float distSq = toEnemy.LengthSquared();
        if (distSq > arcRangeSq)
        {
            continue;
        }

        if (m_arcAngle < 360.0f && distSq > 0.0001f)
        {
            Vector3 toEnemyNorm = toEnemy;
            toEnemyNorm.Normalize();
            if (myForward.Dot(toEnemyNorm) < halfAngleCos)
            {
                continue;
            }
        }

        Script* damScript = GameObjectAPI::getScript(enemy, "Damageable");
        if (damScript == nullptr)
        {
            Debug::log("[ARC] '%s' has no Damageable.", GameObjectAPI::getName(enemy));
            continue;
        }

        Damageable* damageable = static_cast<Damageable*>(damScript);
        damageable->takeDamage(damage);
        hit++;
        Debug::log("[ARC] hit '%s'  dmg=%.1f  hp=%.1f/%.1f",
            GameObjectAPI::getName(enemy), damage,
            damageable->getCurrentHp(), damageable->getMaxHp());
    }

    if (scanned == 0)
    {
        Debug::log("[ARC] no ENEMY tagged objects in scene.");
    }
    else if (hit == 0)
    {
        Debug::log("[ARC] 0 hits — %d enemies scanned, none in range/angle.", scanned);
    }
}

IMPLEMENT_SCRIPT(DeathCharacter)

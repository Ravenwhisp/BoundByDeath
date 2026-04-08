#include "pch.h"
#include "Bound.h"
#include "Damageable.h"

static const ScriptFieldInfo boundFields[] =
{
    { "Min Distance",       ScriptFieldType::Float,        offsetof(Bound, m_minDistance),       {}, {}, {} },
    { "Damage Distance",    ScriptFieldType::Float,        offsetof(Bound, m_distanceDamage),    {}, {}, {} },
    { "InstaKill Distance", ScriptFieldType::Float,        offsetof(Bound, m_distanceInstaKill), {}, {}, {} },
    { "Radius Threshold",   ScriptFieldType::Float,        offsetof(Bound, m_radiusThreshold),   {}, {}, {} },

};


IMPLEMENT_SCRIPT_FIELDS(Bound, boundFields)



Damageable* findDamageable(GameObject* gameObject)
{
    if (!gameObject)
    {
        return nullptr;
    }

    Script* script = GameObjectAPI::getScript(gameObject, "PlayerDamageable");
    Damageable* damageable = dynamic_cast<Damageable*>(script);

    if (damageable)
    {
        return damageable;
    }

    script = GameObjectAPI::getScript(gameObject, "Damageable");
    damageable = dynamic_cast<Damageable*>(script);

    return damageable;
}

Bound::Bound(GameObject* owner) : Script(owner)
{
    const std::vector<GameObject*> players = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER, true);
    if (players.size() == 2) 
    {
        m_firstTarget = GameObjectAPI::getTransform(players[0]);
		m_secondTarget = GameObjectAPI::getTransform(players[1]);

        m_firstDamageable = findDamageable(players[0]);
		m_secondDamageable = findDamageable(players[1]);
	}
    else 
    {
        Debug::warn("Bound script requires exactly 2 players in the scene. Found %d.", players.size());
    }
}

void Bound::Start()
{
}

void Bound::Update()
{
    if (!m_firstTarget || !m_secondTarget ||
        !m_firstDamageable || !m_secondDamageable)
        return;

    const Vector3 p1 = TransformAPI::getPosition(m_firstTarget);
    const Vector3 p2 = TransformAPI::getPosition(m_secondTarget);

    // Midpoint
    m_center = (p1 + p2) * 0.5f;

    const float distance = Vector3::Distance(p1, p2);

    const float maxRadius = m_distanceInstaKill * 0.5f;
    m_currentRadius = min(distance * 0.5f + m_radiusThreshold, maxRadius);

    if (m_previousDistance < m_distanceInstaKill && distance >= m_distanceInstaKill)
    {
        m_firstDamageable->kill();
        m_secondDamageable->kill();
        return;
    }


    if (distance > m_minDistance && distance < m_distanceInstaKill)
    {
        const float range = m_distanceInstaKill - m_minDistance;

        // Normalized factor [0..1] using raw math
        float t = (distance - m_minDistance) / range;

        // Manual clamp
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;

        // Linear damage scale
        const float damagePerSecond = baseDamage + (maxDamage - baseDamage) * t;

        const float damage = damagePerSecond * Time::getDeltaTime();

        m_firstDamageable->takeDamage(damage);
        m_secondDamageable->takeDamage(damage);
    }

    m_previousDistance = distance;
}

void Bound::drawGizmo()
{
    if (!m_firstTarget || !m_secondTarget)
    {
        return;
    }

    const Vector3 p1 = TransformAPI::getPosition(m_firstTarget);
    const Vector3 p2 = TransformAPI::getPosition(m_secondTarget);

    const float distance = Vector3::Distance(p1, p2);
    const Vector3 up = Vector3(0.0f, 1.0f, 0.0f);   // XZ plane circle

    DebugDrawAPI::drawLine(p1, p2, Vector3(1.0f, 1.0f, 1.0f));

    DebugDrawAPI::drawPoint(m_center, Vector3(1.0f, 1.0f, 0.0f), 4.0f);

    Vector3 circleColor;
    if (distance <= m_minDistance)
    {
        circleColor = Vector3(0.0f, 1.0f, 0.0f);
    }
    else if (distance < m_distanceDamage)
    {
        circleColor = Vector3(1.0f, 1.0f, 0.0f);
    }
    else if (distance < m_distanceInstaKill)
    {
        circleColor = Vector3(1.0f, 0.3f, 0.0f);
    }
    else
    {
        circleColor = Vector3(1.0f, 1.0f, 1.0f);
    }

    DebugDrawAPI::drawCircle(m_center, up, circleColor, m_currentRadius, 32.0f);


    const float damageRadius = min(m_distanceDamage * 0.5f, m_distanceInstaKill * 0.5f);
    const float instaKillRadius = m_distanceInstaKill * 0.5f;

    DebugDrawAPI::drawCircle(m_center, up, Vector3(1.0f, 1.0f, 0.0f), damageRadius, 32.0f);
    DebugDrawAPI::drawCircle(m_center, up, Vector3(1.0f, 0.0f, 0.0f), instaKillRadius, 32.0f);
}


IMPLEMENT_SCRIPT(Bound)
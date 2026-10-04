#include "pch.h"
#include "SummonerTeleportVFX.h"
#include "ParticleLifecycle.h"

#include <cmath>
#include <cstring>

namespace
{
    constexpr float kTwoPi = 6.28318531f;
    constexpr float kStartHeight = 0.15f;

    float clamp01(float value)
    {
        return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
    }
}

IMPLEMENT_SCRIPT_FIELDS(SummonerTeleportVFX,
    SERIALIZED_STRING(m_streamPrefix, "Stream Name Prefix"),
    SERIALIZED_STRING(m_convergeBurstName, "Converge Burst Name"),
    SERIALIZED_FLOAT(m_duration, "Spiral Duration", 0.05f, 3.0f, 0.01f),
    SERIALIZED_FLOAT(m_radius, "Spiral Radius", 0.1f, 3.0f, 0.05f),
    SERIALIZED_FLOAT(m_coreRadius, "Spiral Core Radius", 0.0f, 1.0f, 0.01f),
    SERIALIZED_FLOAT(m_height, "Spiral Height", 0.2f, 8.0f, 0.1f),
    SERIALIZED_FLOAT(m_turns, "Spiral Turns", 0.25f, 5.0f, 0.05f)
)

SummonerTeleportVFX::SummonerTeleportVFX(GameObject* owner)
    : Script(owner)
{
}

Vector3 SummonerTeleportVFX::helixPoint(float t, float baseAngle, float radius, float coreRadius, float height, float turns)
{
    const float smooth = t * t * (3.0f - 2.0f * t);
    const float move = t * 0.65f + smooth * 0.35f;

    const float tighten = t * t;

    const float r = coreRadius + (radius - coreRadius) * (1.0f - tighten);
    const float y = kStartHeight + height * move;
    const float angle = baseAngle + move * turns * kTwoPi;

    return Vector3(std::cos(angle) * r, y, std::sin(angle) * r);
}

void SummonerTeleportVFX::Start()
{
    findParts();

    m_timer = 0.0f;
    m_converged = false;

    for (size_t i = 0; i < m_streams.size(); ++i)
    {
        TransformAPI::setPosition(m_streams[i], helixPoint(0.0f, m_baseAngles[i], m_radius, m_coreRadius, m_height, m_turns));
    }

    if (m_convergeBurst)
    {
        TransformAPI::setPosition(GameObjectAPI::getTransform(m_convergeBurst), Vector3(0.0f, kStartHeight + m_height, 0.0f));
    }
}

void SummonerTeleportVFX::findParts()
{
    m_streams.clear();
    m_baseAngles.clear();
    m_convergeBurst = nullptr;

    Transform* root = GameObjectAPI::getTransform(getOwner());
    if (!root)
    {
        return;
    }

    const int childCount = TransformAPI::getChildCount(root);
    for (int i = 0; i < childCount; ++i)
    {
        Transform* child = TransformAPI::getChild(root, i);
        GameObject* childObject = child ? ComponentAPI::getOwner(child) : nullptr;
        const char* name = childObject ? GameObjectAPI::getName(childObject) : nullptr;
        if (!name)
        {
            continue;
        }

        if (std::strncmp(name, m_streamPrefix.c_str(), m_streamPrefix.size()) == 0)
        {
            m_streams.push_back(child);
        }
        else if (m_convergeBurstName == name)
        {
            m_convergeBurst = childObject;
        }
    }

    for (size_t i = 0; i < m_streams.size(); ++i)
    {
        m_baseAngles.push_back(kTwoPi * static_cast<float>(i) / static_cast<float>(m_streams.size()));
    }
}

void SummonerTeleportVFX::Update()
{
    if (m_converged)
    {
        return;
    }

    m_timer += Time::getDeltaTime();

    const float duration = m_duration > 0.01f ? m_duration : 0.01f;
    const float t = clamp01(m_timer / duration);

    for (size_t i = 0; i < m_streams.size(); ++i)
    {
        TransformAPI::setPosition(m_streams[i], helixPoint(t, m_baseAngles[i], m_radius, m_coreRadius, m_height, m_turns));
    }

    if (t >= 1.0f)
    {
        m_converged = true;

        if (m_convergeBurst)
        {
            ParticleLifecycle::activate(m_convergeBurst);
        }
    }
}

IMPLEMENT_SCRIPT(SummonerTeleportVFX)

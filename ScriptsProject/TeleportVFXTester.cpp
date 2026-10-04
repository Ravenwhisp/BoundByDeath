#include "pch.h"
#include "TeleportVFXTester.h"

IMPLEMENT_SCRIPT_FIELDS(TeleportVFXTester,
    SERIALIZED_ASSET_REF(m_effectPrefab, "Effect Prefab", AssetType::PREFAB),
    SERIALIZED_FLOAT(m_interval, "Interval", 0.2f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_effectLifetime, "Effect Lifetime", 0.1f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_jumpDistance, "Jump Distance", 0.0f, 20.0f, 0.1f),
    SERIALIZED_BOOL(m_playArrival, "Play Arrival")
)

TeleportVFXTester::TeleportVFXTester(GameObject* owner)
    : Script(owner)
{
}

void TeleportVFXTester::Start()
{
    m_origin = TransformAPI::getGlobalPosition(GameObjectAPI::getTransform(getOwner()));

    m_timer = m_interval;
}

void TeleportVFXTester::OnGameStop()
{
    m_tracker.clear();
}

void TeleportVFXTester::Update()
{
    const float deltaTime = Time::getDeltaTime();
    m_tracker.update(deltaTime);

    m_timer += deltaTime;
    if (m_timer < m_interval)
    {
        return;
    }

    m_timer = 0.0f;
    playTeleport();
}

void TeleportVFXTester::playTeleport()
{
    if (!m_effectPrefab.m_id.isValid())
    {
        Debug::warn("[TeleportVFXTester] Effect Prefab is not assigned.");
        return;
    }

    const Vector3 sideA = m_origin;
    const Vector3 sideB = m_origin + Vector3(m_jumpDistance, 0.0f, 0.0f);

    const Vector3 departure = m_onOtherSide ? sideB : sideA;
    const Vector3 arrival = m_onOtherSide ? sideA : sideB;
    m_onOtherSide = !m_onOtherSide;

    ParticleLifecycle::spawnOneShotTimed(m_tracker, m_effectPrefab.m_id, departure, Vector3::Zero, m_effectLifetime);

    if (m_playArrival)
    {
        ParticleLifecycle::spawnOneShotTimed(m_tracker, m_effectPrefab.m_id, arrival, Vector3::Zero, m_effectLifetime);
    }
}

IMPLEMENT_SCRIPT(TeleportVFXTester)

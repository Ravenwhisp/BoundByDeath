#include "pch.h"
#include "ParticleManager.h"

IMPLEMENT_SCRIPT_FIELDS(ParticleManager,
    SERIALIZED_COMPONENT_REF(m_playerTransform, "Player Transform", ComponentType::TRANSFORM),
    SERIALIZED_FLOAT(m_activationDistance, "Activation Distance", 0.0f, 500.0f, 1.0f),
    SERIALIZED_FLOAT(m_checkIntervalSeconds, "Check Interval (s)", 0.1f, 10.0f, 0.1f)
)

ParticleManager::ParticleManager(GameObject* owner)
    : Script(owner)
{
}

void ParticleManager::Start()
{
    m_timer = 0.0f;
    refreshParticleCache();
    Debug::log("[ParticleManager] Initialized with %zu particle objects, distance=%.1f, interval=%.2fs.",
        m_particleObjects.size(), m_activationDistance, m_checkIntervalSeconds);
}

void ParticleManager::Update()
{
    m_timer += Time::getDeltaTime();
    if (m_timer >= m_checkIntervalSeconds)
    {
        m_timer -= m_checkIntervalSeconds;
        updateActivity();
    }
}

void ParticleManager::refreshParticleCache()
{
    m_particleObjects = SceneAPI::findAllGameObjectsByComponent(ComponentType::PARTICLE_SYSTEM, false);

    GameObject* self = getOwner();
    m_particleObjects.erase(
        std::remove(m_particleObjects.begin(), m_particleObjects.end(), self),
        m_particleObjects.end()
    );
}

void ParticleManager::updateActivity()
{
    Transform* playerT = m_playerTransform.getReferencedComponent();
    if (!playerT) return;

    const Vector3 playerPos = TransformAPI::getGlobalPosition(playerT);

    for (GameObject* obj : m_particleObjects)
    {
        if (!obj) continue;

        Transform* t = GameObjectAPI::getTransform(obj);
        if (!t) continue;

        const float distance = Vector3::Distance(playerPos, TransformAPI::getGlobalPosition(t));
        const bool  shouldBeActive = (distance <= m_activationDistance);

        if (GameObjectAPI::isActiveSelf(obj) != shouldBeActive)
        {
            GameObjectAPI::setActive(obj, shouldBeActive);
        }
    }
}

void ParticleManager::drawGizmo()
{
    Transform* playerT = m_playerTransform.getReferencedComponent();
    if (!playerT) return;

    const Vector3 playerPos = TransformAPI::getGlobalPosition(playerT);
    DebugDrawAPI::drawSphere(playerPos, Vector3(0.0f, 1.0f, 0.5f), m_activationDistance, 0, false);
}

IMPLEMENT_SCRIPT(ParticleManager)

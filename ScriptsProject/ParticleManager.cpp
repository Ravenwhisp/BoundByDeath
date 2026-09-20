#include "pch.h"
#include "ParticleManager.h"

#include "ParticleLifecycle.h"

#include <algorithm>
#include <limits>

ParticleManager* ParticleManager::s_instance = nullptr;
std::vector<GameObject*> ParticleManager::s_pendingRegistrations;

namespace
{
    // visitParticleSystems takes a plain function pointer, so the result is
    // shared through this flag. Registration is not reentrant.
    bool s_foundParticleSystem = false;
    void markParticleSystemFound(ParticleSystemComponent*) { s_foundParticleSystem = true; }
}

IMPLEMENT_SCRIPT_FIELDS(ParticleManager,
    SERIALIZED_FLOAT(m_activationDistance, "Activation Distance", 0.0f, 500.0f, 1.0f),
    SERIALIZED_FLOAT(m_deactivationDistance, "Deactivation Distance", 0.0f, 500.0f, 1.0f),
    SERIALIZED_FLOAT(m_checkIntervalSeconds, "Check Interval (s)", 0.1f, 10.0f, 0.1f),
    SERIALIZED_BOOL(m_manageAllParticles, "Manage All Particles")
)

ParticleManager::ParticleManager(GameObject* owner)
    : Script(owner)
{
}

void ParticleManager::Start()
{
    // Hysteresis requires the exit distance to be the larger one.
    if (m_deactivationDistance < m_activationDistance)
    {
        m_deactivationDistance = m_activationDistance;
    }

    m_timer = 0.0f;
    s_instance = this;
    adoptPendingRegistrations();

    if (m_manageAllParticles)
    {
        scanAndRegisterAll();
    }

    Debug::log("[ParticleManager] Managing %zu VFX roots (manageAll=%s), activate=%.1f, deactivate=%.1f, interval=%.2fs.",
        m_managedParticles.size(), m_manageAllParticles ? "true" : "false",
        m_activationDistance, m_deactivationDistance, m_checkIntervalSeconds);
}

void ParticleManager::OnGameStop()
{
    // Leave every managed object as it was before the manager touched it.
    for (ManagedParticle& entry : m_managedParticles)
    {
        if (entry.deactivatedByManager && entry.gameObject != nullptr && SceneAPI::containsGameObject(entry.gameObject))
        {
            GameObjectAPI::setActive(entry.gameObject, true);
        }
    }

    m_managedParticles.clear();
    s_pendingRegistrations.clear();

    if (s_instance == this)
    {
        s_instance = nullptr;
    }
}

void ParticleManager::Update()
{
    m_timer += Time::getDeltaTime();
    if (m_timer >= m_checkIntervalSeconds)
    {
        m_timer -= m_checkIntervalSeconds;
        pruneInvalidEntries();
        updateActivity();
    }
}

void ParticleManager::registerVfxRoot(GameObject* root)
{
    if (root == nullptr)
    {
        return;
    }

    if (s_instance != nullptr)
    {
        s_instance->registerRoot(root);
        return;
    }

    // No manager in the scene yet: keep the root until one starts.
    if (std::find(s_pendingRegistrations.begin(), s_pendingRegistrations.end(), root) == s_pendingRegistrations.end())
    {
        s_pendingRegistrations.push_back(root);
    }
}

void ParticleManager::unregisterVfxRoot(GameObject* root)
{
    if (root == nullptr)
    {
        return;
    }

    auto pendingIt = std::find(s_pendingRegistrations.begin(), s_pendingRegistrations.end(), root);
    if (pendingIt != s_pendingRegistrations.end())
    {
        s_pendingRegistrations.erase(pendingIt);
    }

    if (s_instance == nullptr)
    {
        return;
    }

    std::vector<ManagedParticle>& entries = s_instance->m_managedParticles;
    for (size_t i = 0; i < entries.size(); ++i)
    {
        if (entries[i].gameObject == root)
        {
            if (entries[i].deactivatedByManager)
            {
                GameObjectAPI::setActive(root, true);
                ParticleLifecycle::restart(root);
            }

            entries.erase(entries.begin() + i);
            return;
        }
    }
}

bool ParticleManager::registerRoot(GameObject* root)
{
    if (!isValidVfxRoot(root))
    {
        return false;
    }

    for (const ManagedParticle& entry : m_managedParticles)
    {
        if (entry.gameObject == root)
        {
            return true;
        }
    }

    ManagedParticle entry;
    entry.gameObject = root;
    m_managedParticles.push_back(entry);
    return true;
}

void ParticleManager::adoptPendingRegistrations()
{
    for (GameObject* root : s_pendingRegistrations)
    {
        registerRoot(root);
    }

    s_pendingRegistrations.clear();
}

void ParticleManager::scanAndRegisterAll()
{
    const std::vector<GameObject*> all = SceneAPI::findAllGameObjectsByComponent(
        ComponentType::PARTICLE_SYSTEM, false);

    for (GameObject* obj : all)
    {
        if (obj == nullptr || obj == getOwner())
        {
            continue;
        }

        // Skip objects already covered by an explicitly registered root.
        if (isUnderManagedRoot(obj))
        {
            continue;
        }

        registerRoot(obj);
    }
}

bool ParticleManager::isUnderManagedRoot(GameObject* obj) const
{
    Transform* transform = GameObjectAPI::getTransform(obj);

    while (transform != nullptr)
    {
        GameObject* current = ComponentAPI::getOwner(transform);
        if (current == nullptr)
        {
            return false;
        }

        for (const ManagedParticle& entry : m_managedParticles)
        {
            if (entry.gameObject == current)
            {
                return true;
            }
        }

        transform = TransformAPI::getParent(transform);
    }

    return false;
}

void ParticleManager::pruneInvalidEntries()
{
    for (size_t i = m_managedParticles.size(); i-- > 0;)
    {
        GameObject* obj = m_managedParticles[i].gameObject;
        if (obj == nullptr || !SceneAPI::containsGameObject(obj))
        {
            m_managedParticles.erase(m_managedParticles.begin() + static_cast<std::ptrdiff_t>(i));
        }
    }
}

bool ParticleManager::isValidVfxRoot(GameObject* root) const
{
    if (root == nullptr || root == getOwner())
    {
        return false;
    }

    if (!SceneAPI::containsGameObject(root))
    {
        return false;
    }

    s_foundParticleSystem = false;
    ParticleLifecycle::visitParticleSystems(root, &markParticleSystemFound);

    if (!s_foundParticleSystem)
    {
        Debug::warn("[ParticleManager] Rejected VFX root '%s': no particle system found in its hierarchy.",
            GameObjectAPI::getName(root));
        return false;
    }

    return true;
}

void ParticleManager::updateActivity()
{
    const std::vector<GameObject*> players = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER);
    if (players.empty())
    {
        return;
    }

    for (ManagedParticle& entry : m_managedParticles)
    {
        GameObject* obj = entry.gameObject;

        Transform* transform = GameObjectAPI::getTransform(obj);
        if (transform == nullptr)
        {
            continue;
        }

        const Vector3 position = TransformAPI::getGlobalPosition(transform);

        float nearestDistance = (std::numeric_limits<float>::max)();
        for (GameObject* player : players)
        {
            Transform* playerTransform = GameObjectAPI::getTransform(player);
            if (playerTransform == nullptr)
            {
                continue;
            }

            const float distance = Vector3::Distance(position, TransformAPI::getGlobalPosition(playerTransform));
            if (distance < nearestDistance)
            {
                nearestDistance = distance;
            }
        }

        if (nearestDistance == (std::numeric_limits<float>::max)())
        {
            continue;
        }

        const bool isActive = GameObjectAPI::isActiveSelf(obj);

        if (isActive && nearestDistance > m_deactivationDistance)
        {
            ParticleLifecycle::stop(obj);
            GameObjectAPI::setActive(obj, false);
            entry.deactivatedByManager = true;
        }
        else if (!isActive && entry.deactivatedByManager && nearestDistance <= m_activationDistance)
        {
            GameObjectAPI::setActive(obj, true);
            ParticleLifecycle::restart(obj);
            entry.deactivatedByManager = false;
        }
        else if (isActive)
        {
            // Reactivated by gameplay or back in range: the manager no longer
            // owns its inactive state.
            entry.deactivatedByManager = false;
        }
    }
}

void ParticleManager::drawGizmo()
{
    std::vector<Vector3> positions;

    for (GameObject* player : SceneAPI::findAllGameObjectsByTag(Tag::PLAYER))
    {
        Transform* playerTransform = GameObjectAPI::getTransform(player);
        if (playerTransform != nullptr)
        {
            positions.push_back(TransformAPI::getGlobalPosition(playerTransform));
        }
    }

    for (const Vector3& position : positions)
    {
        DebugDrawAPI::drawSphere(position, Vector3(0.0f, 1.0f, 0.5f), m_activationDistance, 0, false);
        DebugDrawAPI::drawSphere(position, Vector3(1.0f, 0.6f, 0.1f), m_deactivationDistance, 0, false);
    }
}

IMPLEMENT_SCRIPT(ParticleManager)

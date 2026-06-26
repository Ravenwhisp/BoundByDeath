#include "pch.h"
#include "EnemyStunParticles.h"

IMPLEMENT_SCRIPT_FIELDS(EnemyStunParticles,
    SERIALIZED_STRING(m_stunPrefab, "Stun Particle Prefab"),
    SERIALIZED_FLOAT(m_heightOffset, "Height Offset", 0.0f, 10.0f, 0.1f)
)

EnemyStunParticles::EnemyStunParticles(GameObject* owner) : Script(owner) {}

void EnemyStunParticles::Start()
{
    m_animation = AnimationAPI::getAnimationComponent(getOwner());
}

void EnemyStunParticles::Update()
{
    if (!m_animation || m_stunPrefab.empty()) return;

    const char* state = AnimationAPI::getActiveStateName(m_animation);
    if (!state) return;

    const bool nowInStun = (strcmp(state, "STUN") == 0);

    // Enter stun
    if (nowInStun && !m_inStun)
    {
        Transform* t = GameObjectAPI::getTransform(getOwner());
        Vector3 pos  = t ? TransformAPI::getGlobalPosition(t) : Vector3::Zero;
        pos.y       += m_heightOffset;
        m_stunParticle = GameObjectAPI::instantiatePrefab(m_stunPrefab.c_str(), pos, Vector3::Zero);
        m_inStun = true;
    }

    // Keep particle above enemy while stunned
    if (m_inStun && m_stunParticle)
    {
        Transform* enemyT    = GameObjectAPI::getTransform(getOwner());
        Transform* particleT = GameObjectAPI::getTransform(m_stunParticle);
        if (enemyT && particleT)
        {
            Vector3 pos = TransformAPI::getGlobalPosition(enemyT);
            pos.y      += m_heightOffset;
            TransformAPI::setGlobalPosition(particleT, pos);
        }
    }

    // Exit stun
    if (!nowInStun && m_inStun)
    {
        if (m_stunParticle)
        {
            GameObjectAPI::removeGameObject(m_stunParticle);
            m_stunParticle = nullptr;
        }
        m_inStun = false;
    }
}

IMPLEMENT_SCRIPT(EnemyStunParticles)

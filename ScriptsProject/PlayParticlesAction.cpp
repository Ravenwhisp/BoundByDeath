#include "pch.h"
#include "PlayParticlesAction.h"

IMPLEMENT_SCRIPT_FIELDS_INHERITED(PlayParticlesAction, CameraTransitionStepAction,
    SERIALIZED_ASSET_REF(m_particlePrefab, "Particle Prefab", AssetType::PREFAB),
    SERIALIZED_COMPONENT_REF(m_spawnAt, "Spawn At", ComponentType::TRANSFORM),
    SERIALIZED_VEC3(m_spawnOffset, "Spawn Offset"),
    SERIALIZED_FLOAT(m_lifetime, "Lifetime", 0.0f, 30.0f, 0.05f)
)

PlayParticlesAction::PlayParticlesAction(GameObject* owner)
    : CameraTransitionStepAction(owner)
{
}

void PlayParticlesAction::Update()
{
    CameraTransitionStepAction::Update();

    m_spawned.update(Time::getDeltaTime());
}

void PlayParticlesAction::OnGameStop()
{
    m_spawned.clear();
}

void PlayParticlesAction::executeAction(CameraTransitionController* controller, CameraTransitionStep* step)
{
    if (!m_particlePrefab.m_id.isValid())
    {
        return;
    }

    Vector3 position = m_spawnOffset;

    Transform* spawnAt = m_spawnAt.getReferencedComponent();
    if (spawnAt != nullptr)
    {
        position = TransformAPI::getGlobalPosition(spawnAt) + m_spawnOffset;
    }

    ParticleLifecycle::spawnOneShotTimed(
        m_spawned,
        m_particlePrefab.m_id,
        position,
        Vector3::Zero,
        m_lifetime
    );
}

IMPLEMENT_SCRIPT(PlayParticlesAction)

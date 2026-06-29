#include "pch.h"
#include "DeathParticles.h"


IMPLEMENT_SCRIPT_FIELDS(DeathParticles,
    SERIALIZED_COMPONENT_REF(m_dashTrail, "Dash", ComponentType::TRANSFORM),
    SERIALIZED_COMPONENT_REF(m_scytheTrail, "Scythe", ComponentType::TRANSFORM)
    SERIALIZED_COMPONENT_REF(M_tauntParticle, "Taunt", ComponentType::TRANSFORM)
)

DeathParticles::DeathParticles(GameObject* owner) : Script(owner)
{

}

Transform* DeathParticles::getTransform(ScriptComponentRef<Transform> controller)
{
    Transform* particleTransform = controller.getReferencedComponent();

    if (particleTransform == nullptr)
    {
        Debug::warn("Missing reference on Death Particles on %s.", GameObjectAPI::getName(getOwner()));
        return nullptr;
    }

    return particleTransform;
}


void DeathParticles::SetDashActive()
{
    if (m_dashTrailController == nullptr) 
    {
        m_dashTrailController = getTransform(m_dashTrail);

        if (m_dashTrailController == nullptr)
        {
            Debug::warn("Dash trail controller not found on Death Particles.");
            return;
        }
    }
    
    TrailComponent* trailComponent = TrailAPI::getTrailComponent(ComponentAPI::getOwner(m_dashTrailController));
    TrailAPI::generateTrail(trailComponent, true);
}

void DeathParticles::SetScytheActive()
{
    if (m_scytheTrailController == nullptr)
    {
        m_scytheTrailController = getTransform(m_scytheTrail);

        if (m_scytheTrailController == nullptr)
        {
            Debug::warn("Scythe trail controller not found on Death Particles.");
            return;
        }

    }

    TrailComponent* trailComponent = TrailAPI::getTrailComponent(ComponentAPI::getOwner(m_scytheTrailController));
    TrailAPI::generateTrail(trailComponent, true);
}

void DeathParticles::SetTauntActive()
{
    if (m_tauntParticle == nullptr)
    {
        m_tauntParticle = getTransform(m_tauntParticle);

        if (m_tauntParticle == nullptr)
        {
            Debug::warn("Taunt particle controller not found on Death Particles.");
            return;
        }

    }

    ParticleSystemComponent* particleSystem = ParticleAPI::getParticleSystemComponent(ComponentAPI::getOwner(m_tauntParticle));
    ParticleAPI::play(particleSystem);
}

void DeathParticles::SetDashInactive()
{
    if (m_dashTrailController == nullptr)
    {
        m_dashTrailController = getTransform(m_dashTrail);

        if (m_dashTrailController == nullptr)
        {
            Debug::warn("Dash trail controller not found on Death Particles.");
            return;
        }

    }

    TrailComponent* trailComponent = TrailAPI::getTrailComponent(ComponentAPI::getOwner(m_dashTrailController));
    TrailAPI::generateTrail(trailComponent, false);
}

void DeathParticles::SetScytheInactive()
{
    if (m_scytheTrailController == nullptr)
    {
        m_scytheTrailController = getTransform(m_scytheTrail);

        if (m_scytheTrailController == nullptr)
        {
            Debug::warn("Scythe trail controller not found on Death Particles.");
            return;
        }

    }

    TrailComponent* trailComponent = TrailAPI::getTrailComponent(ComponentAPI::getOwner(m_scytheTrailController));
    TrailAPI::generateTrail(trailComponent, false);
}

void DeathParticles::SetTauntInactive()
{
    if (m_tauntParticle == nullptr)
    {
        m_tauntParticle = getTransform(m_tauntParticle);

        if (m_tauntParticle == nullptr)
        {
            Debug::warn("Taunt particle controller not found on Death Particles.");
            return;
        }

    }

    ParticleSystemComponent* particleSystem = ParticleAPI::getParticleSystemComponent(ComponentAPI::getOwner(m_tauntParticle));
    ParticleAPI::stop(particleSystem);
}

IMPLEMENT_SCRIPT(DeathParticles)
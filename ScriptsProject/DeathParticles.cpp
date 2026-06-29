#include "pch.h"
#include "DeathParticles.h"


IMPLEMENT_SCRIPT_FIELDS(DeathParticles,
    SERIALIZED_COMPONENT_REF(m_dashTrail, "Dash", ComponentType::TRANSFORM),
    SERIALIZED_COMPONENT_REF(m_scytheTrail, "Scythe", ComponentType::TRANSFORM)
)

DeathParticles::DeathParticles(GameObject* owner) : Script(owner)
{

}

GameObject* DeathParticles::getObject(ScriptComponentRef<Transform> controller)
{
    Transform* particleTransform = controller.getReferencedComponent();

    if (particleTransform == nullptr)
    {
        Debug::warn("Missing reference on Death Particles on %s.", GameObjectAPI::getName(getOwner()));
        return nullptr;
    }

    GameObject* particleObject = ComponentAPI::getOwner(particleTransform);

    if (particleObject == nullptr)
    {
        return nullptr;
    }

    return particleObject;

}

void DeathParticles::SetDashActive()
{
    if (dashTrailController == nullptr) 
    {
        dashTrailController = getObject(m_dashTrail);

        if (dashTrailController == nullptr)
        {
            Debug::warn("Dash trail controller not found on Death Particles.");
            return;
        }
    }

    GameObjectAPI::setActive(dashTrailController, true);
}

void DeathParticles::SetScytheActive()
{
    if (scytheTrailController == nullptr)
    {
        scytheTrailController = getObject(m_scytheTrail);

        if (scytheTrailController == nullptr)
        {
            Debug::warn("Scythe trail controller not found on Death Particles.");
            return;
        }

    }

    GameObjectAPI::setActive(scytheTrailController, true);
}

void DeathParticles::SetDashInactive()
{
    if (dashTrailController == nullptr)
    {
        dashTrailController = getObject(m_dashTrail);

        if (dashTrailController == nullptr)
        {
            Debug::warn("Dash trail controller not found on Death Particles.");
            return;
        }

    }

    GameObjectAPI::setActive(dashTrailController, false);
}

void DeathParticles::SetScytheInactive()
{
    if (scytheTrailController == nullptr)
    {
        scytheTrailController = getObject(m_scytheTrail);

        if (scytheTrailController == nullptr)
        {
            Debug::warn("Scythe trail controller not found on Death Particles.");
            return;
        }

    }

    GameObjectAPI::setActive(scytheTrailController, false);
}

IMPLEMENT_SCRIPT(DeathParticles)
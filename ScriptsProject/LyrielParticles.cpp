#include "pch.h"
#include "LyrielParticles.h"


IMPLEMENT_SCRIPT_FIELDS(LyrielParticles,
    SERIALIZED_COMPONENT_REF(m_dashTrail, "Dash", ComponentType::TRANSFORM)
)

LyrielParticles::LyrielParticles(GameObject* owner) : Script(owner)
{
}

GameObject* LyrielParticles::getObject(ScriptComponentRef<Transform> controller)
{
    Transform* particleTransform = controller.getReferencedComponent();

    if (particleTransform == nullptr)
    {
        Debug::warn("Missing reference on Lyriel Particles on %s.", GameObjectAPI::getName(getOwner()));
        return nullptr;
    }

    GameObject* particleObject = ComponentAPI::getOwner(particleTransform);

    if (particleObject == nullptr)
    {
        return nullptr;
    }

    return particleObject;

}

void LyrielParticles::SetDashActive()
{
    if (dashTrailController == nullptr)
    {
        dashTrailController = getObject(m_dashTrail);

        if (dashTrailController == nullptr)
        {
            Debug::warn("Dash trail controller not found on Lyriel Particles.");
            return;
        }

    }

    GameObjectAPI::setActive(dashTrailController, true);
}

void LyrielParticles::SetDashInactive()
{
    if (dashTrailController == nullptr)
    {
        dashTrailController = getObject(m_dashTrail);

        if (dashTrailController == nullptr)
        {
            Debug::warn("Dash trail controller not found on Lyriel Particles.");
            return;
        }

    }

    GameObjectAPI::setActive(dashTrailController, false);
}

IMPLEMENT_SCRIPT(LyrielParticles)
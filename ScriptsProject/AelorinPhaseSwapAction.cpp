#include "pch.h"
#include "AelorinPhaseSwapAction.h"

#include "AelorinCinematics.h"

IMPLEMENT_SCRIPT_FIELDS_INHERITED(AelorinPhaseSwapAction, CameraTransitionStepAction,
    SERIALIZED_COMPONENT_REF(m_bossTransform, "Boss Transform", ComponentType::TRANSFORM)
)

AelorinPhaseSwapAction::AelorinPhaseSwapAction(GameObject* owner)
    : CameraTransitionStepAction(owner)
{
}

void AelorinPhaseSwapAction::executeAction(CameraTransitionController* controller, CameraTransitionStep* step)
{
    Transform* bossTransform = m_bossTransform.getReferencedComponent();
    if (!bossTransform)
    {
        Debug::warn("AelorinPhaseSwapAction on '%s' has no Boss Transform assigned.", GameObjectAPI::getName(getOwner()));
        return;
    }

    GameObject* bossObject = ComponentAPI::getOwner(bossTransform);
    if (!bossObject)
    {
        return;
    }

    AelorinCinematics* cinematics = GameObjectAPI::findScript<AelorinCinematics>(bossObject);
    if (!cinematics)
    {
        Debug::warn("AelorinPhaseSwapAction on '%s' could not find AelorinCinematics on the referenced boss.", GameObjectAPI::getName(getOwner()));
        return;
    }

    cinematics->performPhaseTeleportToCenter();
}

IMPLEMENT_SCRIPT(AelorinPhaseSwapAction)

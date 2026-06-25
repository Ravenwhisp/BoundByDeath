#include "pch.h"
#include "PlayAnimationAction.h"

#include "CameraTransitionController.h"
#include "CameraTransitionStep.h"

IMPLEMENT_SCRIPT_FIELDS_INHERITED(PlayAnimationAction, CameraTransitionStepAction,
    SERIALIZED_COMPONENT_REF(m_animationTarget, "Animation Target", ComponentType::TRANSFORM),
    SERIALIZED_STRING(m_stateName, "State Name")
)

PlayAnimationAction::PlayAnimationAction(GameObject* owner)
    : CameraTransitionStepAction(owner)
{
}

void PlayAnimationAction::onStepStarted(CameraTransitionController* controller, CameraTransitionStep* step)
{
    if (!shouldRunOnStepStarted())
    {
        return;
    }

    tryPlayAnimation();
}

void PlayAnimationAction::onStepReached(CameraTransitionController* controller, CameraTransitionStep* step)
{
    if (!shouldRunOnStepReached())
    {
        return;
    }

    tryPlayAnimation();
}

void PlayAnimationAction::onStepFinished(CameraTransitionController* controller, CameraTransitionStep* step)
{
    if (!shouldRunOnStepFinished())
    {
        return;
    }

    tryPlayAnimation();
}

void PlayAnimationAction::tryPlayAnimation()
{
    if (m_stateName.empty())
    {
        Debug::warn("PlayAnimationAction on '%s' has no State Name assigned.", GameObjectAPI::getName(getOwner()));
        return;
    }

    AnimationComponent* animation = findAnimationComponent();
    if (animation == nullptr)
    {
        Debug::warn("PlayAnimationAction on '%s' could not find AnimationComponent on target.", GameObjectAPI::getName(getOwner()));
        return;
    }

    const bool success = AnimationAPI::playState(animation, m_stateName.c_str(), 0.15f);

    if (!success)
    {
        Debug::warn("PlayAnimationAction on '%s' failed to play animation state '%s'.", GameObjectAPI::getName(getOwner()), m_stateName.c_str());
    }
}

AnimationComponent* PlayAnimationAction::findAnimationComponent() const
{
    Transform* targetTransform = m_animationTarget.getReferencedComponent();
    if (targetTransform == nullptr)
    {
        return nullptr;
    }

    GameObject* targetObject = ComponentAPI::getOwner(targetTransform);

    return AnimationAPI::getAnimationComponent(targetObject);
}

IMPLEMENT_SCRIPT(PlayAnimationAction)
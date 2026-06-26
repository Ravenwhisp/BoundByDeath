#include "pch.h"
#include "ArcherFaceTarget.h"
#include "RangedEnemyController.h"

ArcherFaceTarget::ArcherFaceTarget(GameObject* owner) : Script(owner) {}

void ArcherFaceTarget::Start()
{
    m_animation  = AnimationAPI::getAnimationComponent(getOwner());
    m_controller = GameObjectAPI::findScript<RangedEnemyController>(getOwner());
}

void ArcherFaceTarget::Update()
{
    if (!m_animation || !m_controller) return;

    const char* state = AnimationAPI::getActiveStateName(m_animation);
    if (!state) return;

    const bool attacking = (strcmp(state, "BASIC_ATTACK") == 0)
                        || (strcmp(state, "ARROWBARRAGE")  == 0);
    if (!attacking) return;

    Transform* targetT = m_controller->getCurrentTarget();
    if (!targetT) return;

    Transform* rootT = GameObjectAPI::getTransform(getOwner());
    if (!rootT) return;

    Vector3 targetPos = TransformAPI::getGlobalPosition(targetT);
    TransformAPI::lookAt(rootT, targetPos);
}

IMPLEMENT_SCRIPT(ArcherFaceTarget)

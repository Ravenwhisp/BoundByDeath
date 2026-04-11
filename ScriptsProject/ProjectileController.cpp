#include "pch.h"
#include "ProjectileController.h"

static const ScriptFieldInfo projectileControllerFields[] =
{
    { "Min Speed", ScriptFieldType::Float, offsetof(ProjectileController, m_minSpeed), { 0.0f, 150.f, 0.1f } },
    { "Max Speed", ScriptFieldType::Float, offsetof(ProjectileController, m_maxSpeed), { 0.0f, 150.f, 0.1f } },
    { "Scale", ScriptFieldType::Float, offsetof(ProjectileController, m_scale), { 0.0f, 1.f, 0.1f } },
};

IMPLEMENT_SCRIPT_FIELDS(ProjectileController, projectileControllerFields)

ProjectileController::ProjectileController(GameObject* owner)
    : Script(owner)
{
    m_objectTransform = GameObjectAPI::getTransform(owner);

}

void ProjectileController::Start()
{
    m_speed = m_minSpeed + m_scale * (m_maxSpeed - m_minSpeed);

    m_canPierce = (m_speed == m_maxSpeed);
}

void ProjectileController::Update()
{
    /*
    if (Transform* targetTransform = m_target.getReferencedComponent()) 
    {
        Vector3 targetDirection = TransformAPI::getPosition(targetTransform) - TransformAPI::getPosition(m_objectTransform);
        targetDirection.Normalize();
        
        Vector3 up = Vector3(0.f, 1.f, 0.f); // we assume projectiles not upside-down
        Quaternion lookToTargetRotation = Quaternion::LookRotation(targetDirection, up);

        TransformAPI::setRotationEuler(m_objectTransform, lookToTargetRotation.ToEuler()); // I think we should be able to assing Quaternions...
    }
    */

    Vector3 translation = m_speed*Time::getDeltaTime() * TransformAPI::getForward(m_objectTransform);
    TransformAPI::translate(m_objectTransform, translation);

    if (hit()) 
    {
        if (m_canPierce) m_canPierce = false;
        else GameObjectAPI::removeGameObject(m_objectTransform->getOwner());
    }
}

bool ProjectileController::hit()
{
    return false;
}

IMPLEMENT_SCRIPT(ProjectileController)

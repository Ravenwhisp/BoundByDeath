#include "pch.h"
#include "ProjectileController.h"

static const ScriptFieldInfo projectileControllerFields[] =
{
    { "Min Speed", ScriptFieldType::Float, offsetof(ProjectileController, m_minSpeed), { 0.0f, 150.f, 0.1f } },
    { "Max Speed", ScriptFieldType::Float, offsetof(ProjectileController, m_maxSpeed), { 0.0f, 150.f, 0.1f } },
    { "Scale", ScriptFieldType::Float, offsetof(ProjectileController, m_scale), { 0.0f, 1.f, 0.1f } },
    { "Target", ScriptFieldType::ComponentRef, offsetof(ProjectileController, m_target), {}, {}, { ComponentType::TRANSFORM } }
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
    Vector3 translation = m_speed*Time::getDeltaTime() * TransformAPI::getForward(m_objectTransform);
    TransformAPI::translate(m_objectTransform, translation);

    if (hit()) 
    {
        if (m_canPierce) m_canPierce = false;
        //else GameObjectAPI::delete();
    }
}

bool ProjectileController::hit()
{
    return false;
}

IMPLEMENT_SCRIPT(ProjectileController)

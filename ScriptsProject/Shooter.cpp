#include "pch.h"
#include "Shooter.h"
#include "ProjectileController.h"

static const ScriptFieldInfo shooterFields[] =
{
    { "Charge Time", ScriptFieldType::Float, offsetof(Shooter, m_chargeTime), { 0.0f, 150.f, 0.1f } },
    { "Target", ScriptFieldType::ComponentRef, offsetof(Shooter, m_targetTransform), {}, {}, { ComponentType::TRANSFORM } },
    {"Prefab to instantiate", ScriptFieldType::String, offsetof(Shooter, m_projectileToInstantiate)  },
    { "Spawning relative point", ScriptFieldType::Vec3, offsetof(Shooter, m_spawnRelativePoint) }
};

IMPLEMENT_SCRIPT_FIELDS(Shooter, shooterFields)

Shooter::Shooter(GameObject* owner)
    : Script(owner)
{
    m_objectTransform = GameObjectAPI::getTransform(owner);
}

void Shooter::Start()
{
    m_isCharging = m_toReleaseCharge = false;
    m_currentChargeTime = 0;
}

void Shooter::Update()
{
    if (m_isCharging)
    {

        if (m_toReleaseCharge) 
        {
            Vector3 objectPosition = TransformAPI::getPosition(m_objectTransform);

            Vector3 targetDirection = TransformAPI::getPosition(m_targetTransform.getReferencedComponent()) - objectPosition;
            targetDirection.Normalize();

            // We calculate the new axis respect the targetDirection, assuming that it represents the x axis (this is so that m_spawnRelativePoint is relative to it)
            Vector3 zDirection = targetDirection.Cross(Vector3(0.f, 1.f, 0.f)); // assuming up = (0.f, 1.f, 0.f)
            zDirection.Normalize();
            Vector3 yDirection = zDirection.Cross(targetDirection);
            yDirection.Normalize();

            Vector3 spawningPosition = objectPosition + 
                m_spawnRelativePoint.x * targetDirection + m_spawnRelativePoint.y * yDirection + m_spawnRelativePoint.z * zDirection;


            GameObject* projectile = GameObjectAPI::instantiatePrefab(m_projectileToInstantiate.c_str(), spawningPosition, TransformAPI::getEulerDegrees(m_objectTransform));
            ProjectileController* projectileScript = static_cast<ProjectileController*>(GameObjectAPI::getScript(projectile, "ProjectileController"));
            projectileScript->m_target.component = m_targetTransform.getReferencedComponent();
            projectileScript->m_scale = m_currentChargeTime / m_chargeTime;


            m_isCharging = m_toReleaseCharge = false;
        }

        else if (m_currentChargeTime != m_chargeTime)
        {

            m_currentChargeTime += Time::getDeltaTime();
            if (m_currentChargeTime > m_chargeTime) m_currentChargeTime = m_chargeTime;
        }

    }
}

// MAYBE ADD IFs SO THAT THESE DO NOT DO ANYTHING BEYOND WHEN IT IS REQUIRED?
void Shooter::startCharge()
{
    m_isCharging = true;
}

void Shooter::releaseCharge()
{
    m_toReleaseCharge = true;
}

void Shooter::releaseImmediately()
{
    m_isCharging = true;
    m_toReleaseCharge = true;
}


IMPLEMENT_SCRIPT(Shooter)
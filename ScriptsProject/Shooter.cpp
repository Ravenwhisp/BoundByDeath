#include "pch.h"
#include "Shooter.h"

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
            // release code
            Vector3 targetDirection = TransformAPI::getPosition(m_targetTransform.getReferencedComponent()) - TransformAPI::getPosition(m_objectTransform);
            targetDirection.Normalize();



            m_isCharging = m_toReleaseCharge = false;
        }
        else 
        {
            if (m_currentChargeTime == m_chargeTime) return;

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
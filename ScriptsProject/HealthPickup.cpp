#include "pch.h"
#include "HealthPickup.h"
#include "PlayerDamageable.h"

IMPLEMENT_SCRIPT(HealthPickup)

IMPLEMENT_SCRIPT_FIELDS(HealthPickup,
    SERIALIZED_FLOAT(m_healAmount,   "Heal Amount",   0.0f, 100.0f, 1.0f),
    SERIALIZED_FLOAT(m_pickupRadius, "Pickup Radius", 0.0f,  10.0f, 0.1f)
)

HealthPickup::HealthPickup(GameObject* owner)
    : Script(owner)
{
}

void HealthPickup::Start()
{
}

void HealthPickup::Update()
{
    
}

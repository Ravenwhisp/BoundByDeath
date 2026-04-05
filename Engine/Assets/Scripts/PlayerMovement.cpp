#include "pch.h"
#include "PlayerMovement.h"

static const ScriptFieldInfo playerMovementFields[]
{
	{ "Move Speed", ScriptFieldType::Float, offsetof(PlayerMovement, m_moveSpeed), { 0.0f, 50.0f, 0.05f } },
	{ "Shift Multiplier", ScriptFieldType::Float, offsetof(PlayerMovement, m_shiftMultiplier), { 1.0f, 10.0f, 0.05f } },
	{ "Constrain To NavMesh", ScriptFieldType::Bool, offsetof(PlayerMovement, m_constrainToNavMesh) },
	{ "Nav Extents", ScriptFieldType::Vec3, offsetof(PlayerMovement, m_navExtents) }
};

IMPLEMENT_SCRIPT_FIELDS(PlayerMovement, playerMovementFields)

PlayerMovement::PlayerMovement(GameObject* owner)
	: Script(owner)
{
}

void PlayerMovement::Start()
{
	GameObject* owner = getOwner();
}

void PlayerMovement::Update()
{
}

void PlayerMovement::applyTranslation(GameObject* owner, const Vector3& direction, float dt, bool shiftHeld) const
{
    Transform* transform = GameObjectAPI::getTransform(owner);
    if (!transform)
    {
        return;
    }

    float speed = m_moveSpeed;
    if (shiftHeld)
    {
        speed *= m_shiftMultiplier;
    }

    const Vector3 currentPos = TransformAPI::getPosition(transform);
    const Vector3 desiredPos = currentPos + direction * speed * dt;

    if (!m_constrainToNavMesh)
    {
        TransformAPI::setPosition(transform, desiredPos);
        return;
    }

    Vector3 constrainedPos;
    if (NavigationAPI::moveAlongSurface(currentPos, desiredPos, constrainedPos, m_navExtents))
    {
        TransformAPI::setPosition(transform, constrainedPos);
    }
}

IMPLEMENT_SCRIPT(PlayerMovement)
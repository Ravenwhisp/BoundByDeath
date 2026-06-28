#include "pch.h"
#include "SkeletonAttackDebugDraw.h"

#include "SkeletonAttackConfig.h"

IMPLEMENT_SCRIPT_FIELDS(SkeletonAttackDebugDraw,
	SERIALIZED_BOOL(m_debugEnabled, "Debug Enabled"),
	SERIALIZED_BOOL(m_drawScimitarStartRange, "Draw Scimitar Start Range"),
	SERIALIZED_BOOL(m_drawDashStopRange, "Draw Dash Stop Range"),
	SERIALIZED_BOOL(m_drawBasicAttackRange, "Draw Basic Attack Range"),
	SERIALIZED_BOOL(m_drawFinalHitRange, "Draw Final Hit Range"),
	SERIALIZED_FLOAT(m_heightOffset, "Height Offset", 0.0f, 5.0f, 0.05f)
)

SkeletonAttackDebugDraw::SkeletonAttackDebugDraw(GameObject* owner)
	: Script(owner)
{
}

void SkeletonAttackDebugDraw::Start()
{
	m_attackConfig = GameObjectAPI::findScript<SkeletonAttackConfig>(getOwner());

	if (!m_attackConfig)
	{
		Debug::warn("[SkeletonAttackDebugDraw] SkeletonAttackConfig not found.");
	}
}

void SkeletonAttackDebugDraw::drawGizmo()
{
	if (!m_debugEnabled)
	{
		return;
	}

	if (!m_attackConfig)
	{
		m_attackConfig = GameObjectAPI::findScript<SkeletonAttackConfig>(getOwner());
	}

	if (!m_attackConfig)
	{
		return;
	}

	Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
	if (!ownerTransform)
	{
		return;
	}

	Vector3 position = TransformAPI::getGlobalPosition(ownerTransform);
	position.y += m_heightOffset;

	const Vector3 up = Vector3(0.0f, 1.0f, 0.0f);

	const Vector3 startRangeColor = Vector3(0.0f, 1.0f, 1.0f);
	const Vector3 dashStopColor = Vector3(1.0f, 1.0f, 0.0f);
	const Vector3 basicAttackColor = Vector3(1.0f, 0.0f, 0.0f);
	const Vector3 finalHitColor = Vector3(1.0f, 0.0f, 1.0f);

	if (m_drawScimitarStartRange)
	{
		DebugDrawAPI::drawCircle(
			position,
			up,
			startRangeColor,
			m_attackConfig->m_scimitarStartRange,
			32.0f,
			0,
			true
		);
	}

	if (m_drawDashStopRange)
	{
		DebugDrawAPI::drawCircle(
			position,
			up,
			dashStopColor,
			m_attackConfig->m_scimitarDashStopRange,
			32.0f,
			0,
			true
		);
	}

	if (m_drawBasicAttackRange)
	{
		DebugDrawAPI::drawCircle(
			position,
			up,
			basicAttackColor,
			m_attackConfig->m_basicAttackRange,
			32.0f,
			0,
			true
		);
	}

	if (m_drawFinalHitRange)
	{
		DebugDrawAPI::drawCircle(
			position,
			up,
			finalHitColor,
			m_attackConfig->m_scimitarStunHitRange,
			32.0f,
			0,
			true
		);
	}
}

IMPLEMENT_SCRIPT(SkeletonAttackDebugDraw)
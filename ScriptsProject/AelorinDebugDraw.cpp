#include "pch.h"
#include "AelorinDebugDraw.h"

#include "AelorinAttackConfig.h"
#include "AelorinBossController.h"

IMPLEMENT_SCRIPT_FIELDS(AelorinDebugDraw,
	FIELD_GROUP_COLLAPSE("Seeker Sigils",
		SERIALIZED_BOOL(m_drawSeekerSigilsNormal, "Draw Normal Impact Areas"),
		SERIALIZED_BOOL(m_drawSeekerSigilsLarge, "Draw Large Impact Areas")
	),

	SERIALIZED_BOOL(m_drawNova, "Draw Nova"),

	SERIALIZED_BOOL(m_debugEnabled, "Debug Enabled"),
	SERIALIZED_FLOAT(m_heightOffset, "Height Offset", 0.0f, 5.0f, 0.05f)
)


AelorinDebugDraw::AelorinDebugDraw(GameObject* owner)
	: Script(owner)
{
}

void AelorinDebugDraw::Start()
{
	m_controller = GameObjectAPI::findScript<AelorinBossController>(getOwner());

	if (!m_controller)
	{
		Debug::warn("[AelorinDebugDraw] AelorinBossController not found.");
	}
}

void AelorinDebugDraw::drawGizmo()
{
	if (!m_debugEnabled)
	{
		return;
	}

	if (!m_controller)
	{
		return;
	}

	const AelorinAttackConfig* config = m_controller->getAelorinAttackConfig();

	if (!config)
	{
		return;
	}

	const Vector3 normalColor =	Vector3(0.0f, 1.0f, 1.0f);
	const Vector3 phase2FinalColor = Vector3(1.0f, 0.0f, 0.0f);
	const Vector3 cyan = Vector3(0.0f, 1.0f, 1.0f);
	const Vector3 orange = Vector3(1.0f, 0.5f, 0.0f);
	const Vector3 yellow = Vector3(1.0f, 1.0f, 0.0f);

	if (m_drawSeekerSigilsNormal)
	{
		drawImpactCircle(m_controller->getLyrielPosition(), config->m_seekerSigilsRadius, normalColor);
		drawImpactCircle(m_controller->getDeathPosition(), config->m_seekerSigilsRadius, normalColor);
	}

	if (m_drawSeekerSigilsLarge)
	{
		const Vector3 lyrielPosition = m_controller->getLyrielPosition();
		const Vector3 deathPosition = m_controller->getDeathPosition();
		const Vector3 midpoint = (lyrielPosition + deathPosition) * 0.5f;

		drawImpactCircle(midpoint, config->m_seekerSigilsPhase2FinalRadius,	phase2FinalColor);
	}

	if (m_drawNova)
	{
		Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
		if (!ownerTransform)
		{
			return;
		}

		const Vector3 bossPosition = TransformAPI::getGlobalPosition(ownerTransform);
		drawImpactCircle(bossPosition, config->m_novaTriggerDistance, phase2FinalColor); // trigger distance
		drawImpactCircle(bossPosition, config->m_novaRadius, cyan); // first wave radius
		drawImpactCircle(bossPosition, config->m_novaPhase2SecondRadius, orange); // second wave radius
	}
}

void AelorinDebugDraw::drawImpactCircle(const Vector3& position, float radius, const Vector3& color) const
{
	Vector3 debugPosition = position;
	debugPosition.y += m_heightOffset;

	DebugDrawAPI::drawCircle(
		debugPosition,
		Vector3(0.0f, 1.0f, 0.0f),
		color,
		radius,
		32.0f,
		0,
		true
	);

	DebugDrawAPI::drawPoint(debugPosition, color, 8.0f, 0,true);
}

IMPLEMENT_SCRIPT(AelorinDebugDraw)
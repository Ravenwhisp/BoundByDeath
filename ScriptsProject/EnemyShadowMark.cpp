#include "pch.h"
#include "EnemyShadowMark.h"
#include "ReaperGauge.h"

#include <cmath>

IMPLEMENT_SCRIPT_FIELDS(EnemyShadowMark, 
    SERIALIZED_FLOAT(m_markDuration, "Mark Duration", 0.5f, 10.0f, 0.1f),
	SERIALIZED_FLOAT(m_markUIScale, "Mark UI Scale", 0.5f, 0.5f, 0.5f),
	SERIALIZED_COMPONENT_REF(m_canvas, "Canvas Transform", ComponentType::TRANSFORM2D),
	SERIALIZED_COMPONENT_REF(m_mark_1, "Mark Phase 1", ComponentType::TRANSFORM),
	SERIALIZED_COMPONENT_REF(m_mark_2, "Mark Phase 2", ComponentType::TRANSFORM),
	SERIALIZED_COMPONENT_REF(m_mark_3, "Mark Phase 3", ComponentType::TRANSFORM)
)

EnemyShadowMark::EnemyShadowMark(GameObject* owner)
    : Script(owner)
{
}

void EnemyShadowMark::Start()
{
	m_canvasTransform2D = m_canvas.getReferencedComponent();
	m_mark1Object = m_mark_1.getReferencedComponent() ? ComponentAPI::getOwner(m_mark_1.getReferencedComponent()) : nullptr;
	m_mark2Object = m_mark_2.getReferencedComponent() ? ComponentAPI::getOwner(m_mark_2.getReferencedComponent()) : nullptr;
	m_mark3Object = m_mark_3.getReferencedComponent() ? ComponentAPI::getOwner(m_mark_3.getReferencedComponent()) : nullptr;
    updateUI();
}

void EnemyShadowMark::Update()
{
    if (m_phase == 0)
        return;

    if (m_canvasTransform2D)
    {
		const float t = (m_markDuration - m_timer) / m_markDuration;
		const float easedTimer = MathAPI::evaluateEasing(MathAPI::EasingType::EaseOutCubic, t);
		Transform2DAPI::setPosition(m_canvasTransform2D, { 0.0f, 1.0f + easedTimer * 0.7f });

		const float pingpongT = MathAPI::pingPong(easedTimer);
		const float scale = 1.0f + pingpongT * 0.5f;
		Transform2DAPI::setScale(m_canvasTransform2D, { scale * m_markUIScale, scale * m_markUIScale });
	}

    m_timer -= Time::getDeltaTime();
    if (m_timer <= 0.0f)
    {
        m_phase = 0;
        m_timer = 0.0f;
        updateUI();
        Debug::log("[ShadowMark] Mark expired.");
    }
}

void EnemyShadowMark::notifyDeathHit()
{
    if (m_phase < 3)
    {
        m_phase++;
        updateUI();
    }

    m_timer = m_markDuration;
    Debug::log("[ShadowMark] Phase %d  timer reset.", m_phase);
}

void EnemyShadowMark::exploit()
{
    Debug::log("[ShadowMark] Mark exploited at phase %d!", m_phase);

    std::vector<GameObject*> players = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER, true);
    for (GameObject* player : players)
    {
        Script* gaugeScript = GameObjectAPI::getScript(player, "ReaperGauge");
        if (gaugeScript != nullptr)
        {
            static_cast<ReaperGauge*>(gaugeScript)->onMarkExploited();
            break;
        }
    }

    m_phase = 0;
    m_timer = 0.0f;
}

void EnemyShadowMark::updateUI()
{
    if (m_mark1Object) GameObjectAPI::setActive(m_mark1Object, m_phase == 1);
    if (m_mark2Object) GameObjectAPI::setActive(m_mark2Object, m_phase == 2);
	if (m_mark3Object) GameObjectAPI::setActive(m_mark3Object, m_phase == 3);
}

void EnemyShadowMark::drawGizmo()
{
    if (m_phase == 0)
        return;

    const Transform* t = GameObjectAPI::getTransform(getOwner());
    if (t == nullptr)
        return;

    Vector3 pos = TransformAPI::getGlobalPosition(t);
    pos.y += 1.8f;

    float   radius;
    Vector3 color;
    switch (m_phase)
    {
    case 1:  radius = 0.20f; color = { 0.35f, 0.35f, 0.35f }; break;
    case 2:  radius = 0.35f; color = { 0.85f, 0.40f, 0.00f }; break;
    default: radius = 0.50f; color = { 0.00f, 0.55f, 1.00f }; break;
    }

    DebugDrawAPI::drawSphere(pos, color, radius, 0, true);

    // Timer ring: white partial arc in XZ plane that shrinks as timer runs out
    if (m_markDuration > 0.0f)
    {
        const float    ratio    = m_timer / m_markDuration;
        const float    ringR    = radius + 0.15f;
        const int      totalSeg = 24;
        const int      fillSeg  = static_cast<int>(ratio * static_cast<float>(totalSeg));
        constexpr float pi2     = 2.0f * 3.14159265f;
        const float    step     = pi2 / static_cast<float>(totalSeg);
        const Vector3  white    = { 1.0f, 1.0f, 1.0f };

        for (int i = 0; i < fillSeg; ++i)
        {
            const float   a0 = step * static_cast<float>(i);
            const float   a1 = a0 + step;
            const Vector3 p0 = { pos.x + cosf(a0) * ringR, pos.y, pos.z + sinf(a0) * ringR };
            const Vector3 p1 = { pos.x + cosf(a1) * ringR, pos.y, pos.z + sinf(a1) * ringR };
            DebugDrawAPI::drawLine(p0, p1, white, 0, true);
        }
    }
}

IMPLEMENT_SCRIPT(EnemyShadowMark)

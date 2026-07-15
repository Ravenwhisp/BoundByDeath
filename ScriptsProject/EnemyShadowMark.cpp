#include "pch.h"
#include "EnemyShadowMark.h"
#include "ReaperGauge.h"
#include <cmath>

IMPLEMENT_SCRIPT_FIELDS(EnemyShadowMark, 
    SERIALIZED_FLOAT(m_markDuration, "Mark Duration", 0.5f, 10.0f, 0.1f),
	SERIALIZED_FLOAT(m_markUITargetScale, "Mark UI Scale", 0.1f, 5.0f, 0.2f),
	SERIALIZED_FLOAT(m_markUIHeightOffset, "Mark UI Height", 0.1f, 50.0f, 20.0f),
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
    if (m_canvasTransform2D)
    {
		m_startScale = Transform2DAPI::getScale(m_canvasTransform2D).x;
	}
	m_mark1Object = m_mark_1.getReferencedComponent() ? ComponentAPI::getOwner(m_mark_1.getReferencedComponent()) : nullptr;
	m_mark2Object = m_mark_2.getReferencedComponent() ? ComponentAPI::getOwner(m_mark_2.getReferencedComponent()) : nullptr;
	m_mark3Object = m_mark_3.getReferencedComponent() ? ComponentAPI::getOwner(m_mark_3.getReferencedComponent()) : nullptr;

	if (!m_canvasTransform2D || !m_mark1Object || !m_mark2Object || !m_mark3Object)
	{
		Transform* ownerTransform = GameObjectAPI::getTransform(getOwner());
		Transform* shadowMarkTransform = TransformAPI::findChildByName(ownerTransform, "Shadow Mark");
		if (shadowMarkTransform)
		{
			GameObject* shadowMarkObject = ComponentAPI::getOwner(shadowMarkTransform);
			if (!m_canvasTransform2D)
			{
				m_canvasTransform2D = static_cast<Transform2D*>(GameObjectAPI::getComponent(shadowMarkObject, ComponentType::TRANSFORM2D));
				if (m_canvasTransform2D)
					m_startScale = Transform2DAPI::getScale(m_canvasTransform2D).x;
			}

			if (!m_mark1Object)
			{
				Transform* mark1 = TransformAPI::findChildByName(shadowMarkTransform, "Shadow Mark 1");
				if (mark1) m_mark1Object = ComponentAPI::getOwner(mark1);
			}
			if (!m_mark2Object)
			{
				Transform* mark2 = TransformAPI::findChildByName(shadowMarkTransform, "Shadow Mark 2");
				if (mark2) m_mark2Object = ComponentAPI::getOwner(mark2);
			}
			if (!m_mark3Object)
			{
				Transform* mark3 = TransformAPI::findChildByName(shadowMarkTransform, "Shadow Mark 3");
				if (mark3) m_mark3Object = ComponentAPI::getOwner(mark3);
			}
		}
	}

    updateUI();
}

void EnemyShadowMark::Update()
{
    updateUI();

    if (m_state == ShadowMarkState::None)
    {
        return;
    }

    m_timer -= Time::getDeltaTime();
    if (m_timer <= 0.0f)
    {
        resetMark();
    }
}

bool EnemyShadowMark::processAttack(EnemyAttackType attackType)
{
    if (m_state == ShadowMarkState::Ready && canExploitWith(attackType))
    {
        exploit();
        return true;
    }

    if (!canApplyWith(attackType))
    {
        return false;
    }

    if (isDeathAttack(attackType))
    {
        applyDeathContribution();
        return false;
    }

    if (isLyrielAttack(attackType))
    {
        applyLyrielContribution();
    }

    return false;
}

void EnemyShadowMark::exploit()
{
    Debug::log("[ShadowMark] Mark exploited");

    if (m_reaperGauge == nullptr)
        m_reaperGauge = findReaperGauge();

    if (m_reaperGauge != nullptr)
        m_reaperGauge->onMarkExploited();
    else
        Debug::warn("[ShadowMark] ReaperGauge not found on any GameObject. Make sure GameController has a ReaperGauge script.");

    resetMark();
}

ReaperGauge* EnemyShadowMark::findReaperGauge()
{
    const std::vector<GameObject*> holders = SceneAPI::findAllGameObjectsWithScript<ReaperGauge>();
    if (holders.empty())
        return nullptr;
    return GameObjectAPI::findScript<ReaperGauge>(holders[0]);
}

void EnemyShadowMark::updateUI()
{
    if (m_mark1Object) 
    {
        GameObjectAPI::setActive(m_mark1Object, m_state == ShadowMarkState::DeathOnly);
    }
    if (m_mark2Object)
    {
        GameObjectAPI::setActive(m_mark2Object, m_state == ShadowMarkState::LyrielOnly);
    }
    if (m_mark3Object)
    {
        GameObjectAPI::setActive(m_mark3Object, m_state == ShadowMarkState::Ready);
    }
    
	if (m_timer <= 0.0f)
    {
        return;
    }

    if (m_canvasTransform2D)
    {
        const float t = (m_markDuration - m_timer) / m_markDuration;
        const float easedTimerPos = MathAPI::evaluateEasing(MathAPI::EasingType::EaseOutCubic, t);
        Transform2DAPI::setPosition(m_canvasTransform2D, { 0.0f, easedTimerPos * m_markUIHeightOffset });

        const float easedTimerScale = MathAPI::evaluateEasing(MathAPI::EasingType::EaseInSine, t);
		const float scale = m_startScale + (m_markUITargetScale - m_startScale) * easedTimerScale;
        Transform2DAPI::setScale(m_canvasTransform2D, { scale, scale });
    }
}

void EnemyShadowMark::drawGizmo()
{
    if (m_state == ShadowMarkState::None)
    {
        return;
    }

    const Transform* t = GameObjectAPI::getTransform(getOwner());
    if (t == nullptr)
        return;

    Vector3 pos = TransformAPI::getGlobalPosition(t);
    pos.y += 1.8f;

    float   radius;
    Vector3 color;
    switch (m_state)
    {
    case ShadowMarkState::DeathOnly:
        radius = 0.20f;
        color = { 0.35f, 0.35f, 0.35f };
        break;

    case ShadowMarkState::LyrielOnly:
        radius = 0.35f;
        color = { 0.85f, 0.40f, 0.00f };
        break;

    case ShadowMarkState::Ready:
        radius = 0.50f;
        color = { 0.00f, 0.55f, 1.00f };
        break;
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

bool EnemyShadowMark::isDeathAttack(EnemyAttackType attackType) const
{
    switch (attackType)
    {
    case EnemyAttackType::DeathBasic:
    case EnemyAttackType::DeathCharged:
    case EnemyAttackType::DeathDash:
    case EnemyAttackType::DeathTaunt:
        return true;

    default:
        return false;
    }
}

bool EnemyShadowMark::isLyrielAttack(EnemyAttackType attackType) const
{
    switch (attackType)
    {
    case EnemyAttackType::LyrielArrow:
    case EnemyAttackType::LyrielVolley:
    case EnemyAttackType::LyrielCharged:
        return true;

    default:
        return false;
    }
}

bool EnemyShadowMark::canApplyWith(EnemyAttackType attackType) const
{
    switch (attackType)
    {
    case EnemyAttackType::DeathBasic:
    case EnemyAttackType::DeathCharged:
    case EnemyAttackType::DeathDash:
    case EnemyAttackType::DeathTaunt:
    case EnemyAttackType::LyrielArrow:
    case EnemyAttackType::LyrielVolley:
    case EnemyAttackType::LyrielCharged:
        return true;

    default:
        return false;
    }
}

bool EnemyShadowMark::canExploitWith(EnemyAttackType attackType) const
{
    switch (attackType)
    {
    case EnemyAttackType::DeathCharged:
    case EnemyAttackType::LyrielCharged:
        return true;

    default:
        return false;
    }
}

void EnemyShadowMark::applyDeathContribution()
{
    switch (m_state)
    {
    case ShadowMarkState::None:
        m_state = ShadowMarkState::DeathOnly;
        break;

    case ShadowMarkState::DeathOnly:
        break;

    case ShadowMarkState::LyrielOnly:
        m_state = ShadowMarkState::Ready;
        break;

    case ShadowMarkState::Ready:
        break;
    }

    resetTimer();
}

void EnemyShadowMark::applyLyrielContribution()
{
    switch (m_state)
    {
    case ShadowMarkState::None:
        m_state = ShadowMarkState::LyrielOnly;
        break;

    case ShadowMarkState::DeathOnly:
        m_state = ShadowMarkState::Ready;
        break;

    case ShadowMarkState::LyrielOnly:
        break;

    case ShadowMarkState::Ready:
        break;
    }

    resetTimer();
}

void EnemyShadowMark::resetTimer()
{
    m_timer = m_markDuration;
}

void EnemyShadowMark::resetMark()
{
    m_state = ShadowMarkState::None;
    m_timer = 0.0f;
    updateUI();
}

IMPLEMENT_SCRIPT(EnemyShadowMark)

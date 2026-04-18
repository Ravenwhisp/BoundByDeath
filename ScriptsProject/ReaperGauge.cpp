#include "pch.h"
#include "ReaperGauge.h"

#include <cmath>

static const ScriptFieldInfo ReaperGaugeFields[] =
{
    { "Max Gauge",         ScriptFieldType::Float, offsetof(ReaperGauge, m_maxGauge),       { 1.0f,  500.0f, 1.0f  } },
    { "Num Segments",      ScriptFieldType::Int,   offsetof(ReaperGauge, m_numSegments),    { 1.0f,   20.0f, 1.0f  } },
    { "Gain Per Exploit",  ScriptFieldType::Float, offsetof(ReaperGauge, m_gainPerExploit), { 0.0f,  100.0f, 1.0f  } },
    { "Grace Period",      ScriptFieldType::Float, offsetof(ReaperGauge, m_gracePeriod),    { 0.0f,   60.0f, 0.5f  } },
    { "Decay Per Second",  ScriptFieldType::Float, offsetof(ReaperGauge, m_decayPerSecond), { 0.0f,   50.0f, 0.5f  } },
};

IMPLEMENT_SCRIPT_FIELDS(ReaperGauge, ReaperGaugeFields)

ReaperGauge::ReaperGauge(GameObject* owner)
    : Script(owner)
{
}

void ReaperGauge::Start()
{
}

void ReaperGauge::Update()
{
    if (!m_everExploited || m_gauge <= 0.0f)
        return;

    m_decayTimer += Time::getDeltaTime();

    if (m_decayTimer > m_gracePeriod)
    {
        m_gauge -= m_decayPerSecond * Time::getDeltaTime();
        if (m_gauge < 0.0f)
            m_gauge = 0.0f;
    }
}

void ReaperGauge::onMarkExploited()
{
    m_everExploited = true;
    m_decayTimer    = 0.0f;

    m_gauge += m_gainPerExploit;
    if (m_gauge > m_maxGauge)
        m_gauge = m_maxGauge;

    Debug::log("[ReaperGauge] +%.1f  gauge=%.1f/%.1f", m_gainPerExploit, m_gauge, m_maxGauge);
}

float ReaperGauge::getGaugePercent() const
{
    if (m_maxGauge <= 0.0f)
        return 0.0f;
    return m_gauge / m_maxGauge;
}

int ReaperGauge::getCurrentSegments() const
{
    if (m_maxGauge <= 0.0f || m_numSegments <= 0)
        return 0;
    const float segValue = m_maxGauge / static_cast<float>(m_numSegments);
    return static_cast<int>(m_gauge / segValue);
}

void ReaperGauge::drawGizmo()
{
    const Transform* t = GameObjectAPI::getTransform(getOwner());
    if (t == nullptr)
        return;

    Vector3 pos = TransformAPI::getGlobalPosition(t);
    pos.y += 2.4f;

    // Segment bars drawn horizontally above Lyriel
    const float segW    = 0.18f;
    const float segH    = 0.08f;
    const float padding = 0.04f;
    const int   segs    = m_numSegments > 0 ? m_numSegments : 1;
    const float total   = segs * segW + (segs - 1) * padding;
    const float startX  = pos.x - total * 0.5f;

    const float gaugePercent = getGaugePercent();
    const float filled       = gaugePercent * static_cast<float>(segs);

    for (int i = 0; i < segs; ++i)
    {
        const float cx = startX + i * (segW + padding) + segW * 0.5f;

        const float segFill = filled - static_cast<float>(i);
        const float ratio   = segFill < 0.0f ? 0.0f : (segFill > 1.0f ? 1.0f : segFill);

        const Vector3 colFilled  = { 0.85f, 0.10f, 0.10f };
        const Vector3 colEmpty   = { 0.30f, 0.30f, 0.30f };
        const Vector3 col        = ratio > 0.0f ? colFilled : colEmpty;

        const Vector3 bl = { cx - segW * 0.5f, pos.y,          pos.z };
        const Vector3 br = { cx + segW * 0.5f, pos.y,          pos.z };
        const Vector3 tl = { cx - segW * 0.5f, pos.y + segH,   pos.z };
        const Vector3 tr = { cx + segW * 0.5f, pos.y + segH,   pos.z };

        DebugDrawAPI::drawLine(bl, br, col, 0, true);
        DebugDrawAPI::drawLine(tl, tr, col, 0, true);
        DebugDrawAPI::drawLine(bl, tl, col, 0, true);
        DebugDrawAPI::drawLine(br, tr, col, 0, true);

        // Fill line inside segment proportional to fill ratio
        if (ratio > 0.0f)
        {
            const float fillRight = cx - segW * 0.5f + segW * ratio;
            const float midY      = pos.y + segH * 0.5f;
            DebugDrawAPI::drawLine(
                { cx - segW * 0.5f, midY, pos.z },
                { fillRight,        midY, pos.z },
                colFilled, 0, true);
        }
    }

    // Grace/decay indicator: small dot above bar, white=grace, red=decaying
    if (m_everExploited && m_gauge > 0.0f)
    {
        const bool    inGrace   = m_decayTimer <= m_gracePeriod;
        const Vector3 dotColor  = inGrace ? Vector3{ 1.0f, 1.0f, 1.0f } : Vector3{ 1.0f, 0.2f, 0.2f };
        const Vector3 dotPos    = { pos.x, pos.y + segH + 0.12f, pos.z };
        DebugDrawAPI::drawPoint(dotPos, dotColor, 5.0f, 0, true);
    }
}

IMPLEMENT_SCRIPT(ReaperGauge)

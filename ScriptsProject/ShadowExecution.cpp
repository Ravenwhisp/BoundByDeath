#include "pch.h"
#include "ShadowExecution.h"
#include "ReaperGauge.h"

static const ScriptFieldInfo ShadowExecutionFields[] =
{
    { "Co-op Window (s)", ScriptFieldType::Float, offsetof(ShadowExecution, m_timeWindow), { 0.5f, 10.0f, 0.5f } },
};

IMPLEMENT_SCRIPT_FIELDS(ShadowExecution, ShadowExecutionFields)

ShadowExecution::ShadowExecution(GameObject* owner)
    : Script(owner)
{
}

void ShadowExecution::Start()
{
    Script* gaugeScript = GameObjectAPI::getScript(getOwner(), "ReaperGauge");
    if (gaugeScript != nullptr)
    {
        m_reaperGauge = static_cast<ReaperGauge*>(gaugeScript);
    }
    else
    {
        Debug::warn("[ShadowExecution] ReaperGauge not found on GameController. Add it as a sibling script.");
    }
}

void ShadowExecution::Update()
{
    if (m_isActive)
        return;

    const float dt = Time::getDeltaTime();

    // Tick down press windows
    const bool p0WasOpen = m_p0Timer > 0.0f;
    const bool p1WasOpen = m_p1Timer > 0.0f;

    if (m_p0Timer > 0.0f) m_p0Timer -= dt;
    if (m_p1Timer > 0.0f) m_p1Timer -= dt;

    // Log window expiry
    if (p0WasOpen && m_p0Timer <= 0.0f)
        Debug::log("[ShadowExecution] Player 0 window expired. Both must press within %.1fs.", m_timeWindow);
    if (p1WasOpen && m_p1Timer <= 0.0f)
        Debug::log("[ShadowExecution] Player 1 window expired. Both must press within %.1fs.", m_timeWindow);

    // Register fresh presses
    if (Input::isFaceButtonTopJustPressed(0))
    {
        m_p0Timer = m_timeWindow;
        const char* gaugeStatus = (m_reaperGauge && m_reaperGauge->isFull()) ? "GAUGE FULL - ready!" : "gauge not full";
        Debug::log("[ShadowExecution] Player 0 pressed Triangle. Window open for %.1fs. [%s]", m_timeWindow, gaugeStatus);
    }
    if (Input::isFaceButtonTopJustPressed(1))
    {
        m_p1Timer = m_timeWindow;
        const char* gaugeStatus = (m_reaperGauge && m_reaperGauge->isFull()) ? "GAUGE FULL - ready!" : "gauge not full";
        Debug::log("[ShadowExecution] Player 1 pressed Triangle. Window open for %.1fs. [%s]", m_timeWindow, gaugeStatus);
    }

    // Both players have pressed within the window
    if (m_p0Timer > 0.0f && m_p1Timer > 0.0f)
        tryTrigger();
}

void ShadowExecution::tryTrigger()
{
    if (m_reaperGauge == nullptr)
    {
        Debug::warn("[ShadowExecution] Cannot trigger: ReaperGauge is null.");
        return;
    }

    if (!m_reaperGauge->isFull())
    {
        Debug::log("[ShadowExecution] Both players ready but gauge not full (%.0f%% - %d/%d segments). Keep exploiting marks!",
            m_reaperGauge->getGaugePercent() * 100.0f,
            m_reaperGauge->getCurrentSegments(),
            m_reaperGauge->m_numSegments);
        return;
    }

    m_isActive = true;
    m_p0Timer  = 0.0f;
    m_p1Timer  = 0.0f;

    Debug::log("[ShadowExecution] *** SHADOW EXECUTION TRIGGERED! ***");
    // TODO: launch ability logic here
}

IMPLEMENT_SCRIPT(ShadowExecution)

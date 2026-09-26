#include "pch.h"
#include "ReaperGauge.h"
#include "CooperativeSound.h"

#include "PersistingCheckpointState.h"

#include <cmath>

IMPLEMENT_SCRIPT_FIELDS(ReaperGauge,
    SERIALIZED_FLOAT(m_maxGauge, "Max Gauge", 1.0f, 500.0f, 1.0f),
    SERIALIZED_INT(m_numSegments, "Num Segments"),
    SERIALIZED_FLOAT(m_gracePeriod, "Grace Period", 0.0f, 60.0f, 0.5f),
    SERIALIZED_FLOAT(m_decayPerSecond, "Decay Per Second", 0.0f, 50.0f, 0.5f),
	SERIALIZED_COMPONENT_REF(m_reaperGaugeUI, "Reaper Gauge UI", ComponentType::UISLIDER),
	SERIALIZED_COMPONENT_REF(m_glowUI, "Glow UI", ComponentType::TRANSFORM2D),
    SERIALIZED_COMPONENT_REF(m_blinkAlphaUI, "Blink Alpha UI", ComponentType::TRANSFORM2D),
    SERIALIZED_FLOAT(m_blinkSpeed, "Blink Speed", 0.1f, 20.0f, 0.1f),
	SERIALIZED_FLOAT(m_blinkAlpha, "Blink Alpha", 0.0f, 1.0f, 0.05f),
    FIELD_GROUP_LABEL("Full Gauge VFX"),
    SERIALIZED_BOOL(m_enableFullVfx, "Enable Full VFX"),
    SERIALIZED_COMPONENT_REF(m_fullGaugeContainer, "Full Gauge Container", ComponentType::TRANSFORM2D),
    SERIALIZED_COMPONENT_REF(m_fullBurstUI, "Full Burst UI", ComponentType::TRANSFORM2D),
    SERIALIZED_COMPONENT_REF(m_fullBurstSheetUI, "Full Burst Sheet", ComponentType::UISHEET),
    SERIALIZED_COMPONENT_REF(m_fullWispsUI, "Full Wisps UI", ComponentType::TRANSFORM2D),
    SERIALIZED_COMPONENT_REF(m_fullWispsSheetUI, "Full Wisps Sheet", ComponentType::UISHEET),
    SERIALIZED_COMPONENT_REF(m_fullShineUI, "Full Shine UI", ComponentType::TRANSFORM2D),
    SERIALIZED_ASSET_REF(m_fullLut, "Full LUT", AssetType::LUT),
    SERIALIZED_FLOAT(m_fullLutFadeInDuration, "Full LUT Fade In Duration", 0.05f, 3.0f, 0.05f),
    SERIALIZED_FLOAT(m_fullLutMinStrength, "Full LUT Minimum Strength", 0.0f, 1.0f, 0.05f),
    SERIALIZED_FLOAT(m_fullLutMaxStrength, "Full LUT Maximum Strength", 0.0f, 1.0f, 0.05f),
    SERIALIZED_FLOAT(m_fullLutBreathingSpeed, "Full LUT Breathing Speed", 0.1f, 5.0f, 0.1f),
    SERIALIZED_FLOAT(m_fullEnterDuration, "Full Enter Duration", 0.05f, 2.0f, 0.05f),
    SERIALIZED_FLOAT(m_fullPopScale, "Full Pop Scale", 1.0f, 1.25f, 0.01f),
    SERIALIZED_FLOAT(m_fullBreathingSpeed, "Full Breathing Speed", 0.1f, 10.0f, 0.1f),
    SERIALIZED_FLOAT(m_fullBreathingIntensity, "Full Breathing Intensity", 0.0f, 0.5f, 0.01f),
    SERIALIZED_FLOAT(m_fullShineInterval, "Full Shine Interval", 0.25f, 10.0f, 0.25f),
    SERIALIZED_FLOAT(m_fullShineDuration, "Full Shine Duration", 0.1f, 2.0f, 0.05f)
)

ReaperGauge::ReaperGauge(GameObject* owner)
    : Script(owner)
{
}

void ReaperGauge::Start()
{
	m_reaperGaugeSlider = m_reaperGaugeUI.getReferencedComponent();
	m_glowTransform = m_glowUI.getReferencedComponent();
	m_blinkAlphaTransform = m_blinkAlphaUI.getReferencedComponent();
	m_fullGaugeTransform = m_fullGaugeContainer.getReferencedComponent();
    m_fullBurstTransform = m_fullBurstUI.getReferencedComponent();
    m_fullBurstSheet = m_fullBurstSheetUI.getReferencedComponent();
    m_fullWispsTransform = m_fullWispsUI.getReferencedComponent();
    m_fullWispsSheet = m_fullWispsSheetUI.getReferencedComponent();
    m_fullShineTransform = m_fullShineUI.getReferencedComponent();

    if (m_fullGaugeTransform)
    {
        m_fullGaugeBaseScale = Transform2DAPI::getScale(m_fullGaugeTransform);
    }

    m_sound = GameObjectAPI::findScript<CooperativeSound>(getOwner());

    PersistingCheckpointState* PersistingCheckpointState = &PersistingCheckpointState::Get();
    if (PersistingCheckpointState && PersistingCheckpointState->m_lastCheckpointId > CheckpointId::NONE)
    {
        m_gauge = PersistingCheckpointState->m_savedReaperGaugeAmount;
        m_everExploited = true;
        m_decayTimer = 0.0f;
        m_decaying = false;
    }

    resetFullVisualComponents();
    m_wasFull = isFull();
    if (m_enableFullVfx && m_wasFull)
    {
        beginFullVisuals(false);
    }
    updateUI();
}

void ReaperGauge::Update()
{
    const float dt = Time::getDeltaTime();

    if (m_everExploited && m_gauge <= 0.0f)
    {
        m_gauge = 0.0f;
    }
    else if (m_everExploited)
    {
        m_decayTimer += dt;

        if (m_decayTimer > m_gracePeriod)
        {
            const bool wasAboveZero = m_gauge > 0.0f;

            if (!m_decaying)
            {
                m_decaying = true;
                Debug::log("[ReaperGauge] Grace period ended. Gauge decaying: %.1f/%.1f", m_gauge, m_maxGauge);
            }

            m_gauge -= m_decayPerSecond * dt;

            if (m_gauge <= 0.0f)
            {
                m_gauge = 0.0f;
                m_decaying = false;
            }

            if (wasAboveZero && m_gauge <= 0.0f)
                Debug::log("[ReaperGauge] Gauge empty. Shadow Execution NOT available.");
        }
    }

    updateUI();
    updateFullVisuals(dt);
}

void ReaperGauge::OnGameStop()
{
    restorePreviousLut();
    resetFullVisualComponents();
}

void ReaperGauge::onMarkExploited()
{
    m_everExploited = true;
    m_decayTimer    = 0.0f;
    m_decaying      = false;

    const bool wasFull = isFull();

    const float segmentValue = m_maxGauge / static_cast<float>(m_numSegments);
    constexpr float epsilon = 0.0001f;
    const int nextSegment = static_cast<int>((m_gauge + epsilon) / segmentValue) + 1;

    m_gauge = static_cast<float>(nextSegment) * segmentValue;;

    if (m_gauge > m_maxGauge)
    {
        m_gauge = m_maxGauge;
    }

    if (!wasFull && isFull())
    {
        Debug::log("[ReaperGauge] GAUGE FULL! Shadow Execution is now available.");
        if (m_sound != nullptr)
        {
            m_sound->playReaperGaugeFull();
        }

        if (m_enableFullVfx)
        {
            beginFullVisuals(true);
        }
    }

    m_wasFull = isFull();
}

void ReaperGauge::consume()
{
    m_gauge      = 0.0f;
    m_decayTimer = 0.0f;
    m_decaying   = false;

    if (m_enableFullVfx)
    {
        endFullVisuals();
    }
    m_wasFull = false;

    updateUI();

    Debug::log("[ReaperGauge] Gauge consumed by Shadow Execution.");
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

void ReaperGauge::updateUI()
{
    if (m_gauge <= 0.0f)
    {
        if (m_reaperGaugeSlider) {
            SliderAPI::setFillAmount(m_reaperGaugeSlider, 0.0f);
        }

        if (m_glowTransform)
        {
            Transform2DAPI::setAlpha(m_glowTransform, 0.0f);
        }

        if (m_blinkAlphaTransform) 
        {
            Transform2DAPI::setAlpha(m_blinkAlphaTransform, 0.0f);
        }

        return;
    }

    if (m_reaperGaugeSlider)
    {
        SliderAPI::setFillAmount(m_reaperGaugeSlider, getGaugePercent());
    }

    if (m_glowTransform && (!m_enableFullVfx || !isFull()))
    {
        float alpha = 0.0f;

        if (m_gracePeriod > 0.0f && m_decayTimer <= m_gracePeriod) {
            alpha = 1.0f - (m_decayTimer / m_gracePeriod);
        }

        Transform2DAPI::setAlpha(m_glowTransform, alpha);
    }

    if (m_blinkAlphaTransform)
    {
        if (m_decayTimer > m_gracePeriod)
        {
            const float t = (sinf((m_decayTimer - m_gracePeriod) * m_blinkSpeed) + 1.0f) * 0.5f;
            Transform2DAPI::setAlpha(m_blinkAlphaTransform, t * m_blinkAlpha);
        }
        else
        {
            Transform2DAPI::setAlpha(m_blinkAlphaTransform, 0.0f);
        }
    }
}

void ReaperGauge::beginFullVisuals(bool playAnnouncement)
{
    m_visualState = playAnnouncement ? ReaperGaugeVisualState::FullEnter : ReaperGaugeVisualState::FullIdle;
    m_fullStateTimer = 0.0f;
    m_shineTimer = 0.0f;
    m_shineAnimTimer = 0.0f;
    m_shineAnimating = false;

    if (m_fullGaugeTransform)
    {
        Transform2DAPI::setScale(m_fullGaugeTransform, m_fullGaugeBaseScale);
    }

    if (m_fullWispsTransform)
    {
        Transform2DAPI::setAlpha(m_fullWispsTransform, 0.58f);
    }
    if (m_fullWispsSheet)
    {
        UISheetAPI::setLoop(m_fullWispsSheet, true);
        UISheetAPI::play(m_fullWispsSheet);
    }

    beginFullLut();

    if (playAnnouncement)
    {
        if (m_fullBurstTransform)
        {
            Transform2DAPI::setAlpha(m_fullBurstTransform, 1.0f);
        }
        if (m_fullBurstSheet)
        {
            UISheetAPI::setLoop(m_fullBurstSheet, false);
            UISheetAPI::play(m_fullBurstSheet);
        }
    }
}

void ReaperGauge::endFullVisuals()
{
    restorePreviousLut();
    resetFullVisualComponents();
    m_visualState = ReaperGaugeVisualState::Normal;
    m_fullStateTimer = 0.0f;
    m_shineTimer = 0.0f;
    m_shineAnimTimer = 0.0f;
    m_shineAnimating = false;
}

void ReaperGauge::updateFullVisuals(float dt)
{
    if (!m_enableFullVfx)
    {
        return;
    }

    const bool full = isFull();
    if (full != m_wasFull)
    {
        if (full)
        {
            beginFullVisuals(true);
        }
        else
        {
            endFullVisuals();
        }
        m_wasFull = full;
    }

    if (!full || m_visualState == ReaperGaugeVisualState::Normal)
    {
        return;
    }

    m_fullStateTimer += dt;
    updateFullLut(dt);

    if (m_visualState == ReaperGaugeVisualState::FullEnter)
    {
        const float duration = m_fullEnterDuration > 0.0f ? m_fullEnterDuration : 0.01f;
        const float t = std::clamp(m_fullStateTimer / duration, 0.0f, 1.0f);
        const float pop = 1.0f + sinf(t * 3.14159265f) * (m_fullPopScale - 1.0f);

        if (m_fullGaugeTransform)
        {
            Transform2DAPI::setScale(m_fullGaugeTransform, Vector2(m_fullGaugeBaseScale.x * pop, m_fullGaugeBaseScale.y * pop));
        }

        if (t >= 1.0f)
        {
            if (m_fullGaugeTransform)
            {
                Transform2DAPI::setScale(m_fullGaugeTransform, m_fullGaugeBaseScale);
            }
            if (m_fullBurstTransform)
            {
                Transform2DAPI::setAlpha(m_fullBurstTransform, 0.0f);
            }
            m_visualState = ReaperGaugeVisualState::FullIdle;
            m_fullStateTimer = 0.0f;
        }
    }

    const float breathing = (sinf(m_fullStateTimer * m_fullBreathingSpeed) + 1.0f) * 0.5f;
    if (m_glowTransform)
    {
        const float glowAlpha = std::clamp(0.62f + (breathing * 2.0f - 1.0f) * m_fullBreathingIntensity, 0.0f, 1.0f);
        Transform2DAPI::setAlpha(m_glowTransform, glowAlpha);
    }

    if (!m_shineAnimating)
    {
        m_shineTimer += dt;
        if (m_shineTimer >= m_fullShineInterval)
        {
            m_shineTimer = 0.0f;
            m_shineAnimTimer = 0.0f;
            m_shineAnimating = true;
        }
    }

    if (m_shineAnimating && m_fullShineTransform)
    {
        m_shineAnimTimer += dt;
        const float duration = m_fullShineDuration > 0.0f ? m_fullShineDuration : 0.01f;
        const float t = std::clamp(m_shineAnimTimer / duration, 0.0f, 1.0f);
        Transform2DAPI::setPosition(m_fullShineTransform, Vector2(-330.0f + 660.0f * t, 0.0f));
        Transform2DAPI::setAlpha(m_fullShineTransform, sinf(t * 3.14159265f) * 0.8f);

        if (t >= 1.0f)
        {
            Transform2DAPI::setAlpha(m_fullShineTransform, 0.0f);
            m_shineAnimating = false;
        }
    }
}

void ReaperGauge::beginFullLut()
{
    if (!m_fullLut.m_id.isValid() || m_fullLut.m_id.m_type != AssetType::LUT || m_lutActive)
    {
        return;
    }

    m_previousLutEnabled = PostProcessAPI::isLutEnabled();
    m_previousLutAsset = PostProcessAPI::getLutAsset();
    m_previousLutStrength = PostProcessAPI::getLutStrength();
    m_lutCaptured = true;
    m_lutActive = true;
    m_fullLutTimer = 0.0f;

    PostProcessAPI::setLutAsset(m_fullLut.m_id);
    PostProcessAPI::setLutStrength(0.0f);
    PostProcessAPI::setLutEnabled(true);
}

void ReaperGauge::updateFullLut(float dt)
{
    if (!m_lutActive || !PostProcessAPI::isLutEnabled() || PostProcessAPI::getLutAsset() != m_fullLut.m_id)
    {
        return;
    }

    m_fullLutTimer += dt;
    const float fadeDuration = m_fullLutFadeInDuration > 0.01f ? m_fullLutFadeInDuration : 0.01f;
    const float fade = std::clamp(m_fullLutTimer / fadeDuration, 0.0f, 1.0f);
    const float lowerStrength = m_fullLutMinStrength < m_fullLutMaxStrength ? m_fullLutMinStrength : m_fullLutMaxStrength;
    const float upperStrength = m_fullLutMinStrength > m_fullLutMaxStrength ? m_fullLutMinStrength : m_fullLutMaxStrength;
    const float minimumStrength = std::clamp(lowerStrength, 0.0f, 1.0f);
    const float maximumStrength = std::clamp(upperStrength, 0.0f, 1.0f);
    const float breathing = (sinf(m_fullLutTimer * m_fullLutBreathingSpeed - 1.57079633f) + 1.0f) * 0.5f;
    PostProcessAPI::setLutStrength(fade * (minimumStrength + (maximumStrength - minimumStrength) * breathing));
}

void ReaperGauge::restorePreviousLut()
{
    if (!m_lutActive)
    {
        return;
    }

    const AssetId currentLut = PostProcessAPI::getLutAsset();
    if (PostProcessAPI::isLutEnabled() && currentLut == m_fullLut.m_id && m_lutCaptured)
    {
        PostProcessAPI::setLutAsset(m_previousLutAsset);
        PostProcessAPI::setLutStrength(m_previousLutStrength);
        PostProcessAPI::setLutEnabled(m_previousLutEnabled);
    }

    m_lutCaptured = false;
    m_lutActive = false;
    m_fullLutTimer = 0.0f;
    m_previousLutAsset = AssetId();
}

void ReaperGauge::resetFullVisualComponents()
{
    if (m_fullGaugeTransform)
    {
        Transform2DAPI::setScale(m_fullGaugeTransform, m_fullGaugeBaseScale);
    }
    if (m_fullBurstTransform)
    {
        Transform2DAPI::setAlpha(m_fullBurstTransform, 0.0f);
    }
    if (m_fullBurstSheet)
    {
        UISheetAPI::stop(m_fullBurstSheet);
        UISheetAPI::reset(m_fullBurstSheet);
    }
    if (m_fullWispsTransform)
    {
        Transform2DAPI::setAlpha(m_fullWispsTransform, 0.0f);
    }
    if (m_fullWispsSheet)
    {
        UISheetAPI::stop(m_fullWispsSheet);
        UISheetAPI::reset(m_fullWispsSheet);
    }
    if (m_fullShineTransform)
    {
        Transform2DAPI::setPosition(m_fullShineTransform, Vector2(-330.0f, 0.0f));
        Transform2DAPI::setAlpha(m_fullShineTransform, 0.0f);
    }
}

void ReaperGauge::drawGizmo()
{
    const Transform* t = GameObjectAPI::getTransform(getOwner());
    if (t == nullptr)
        return;

    const Vector3 pos = TransformAPI::getGlobalPosition(t);

    // Segmented bar flat in XZ plane, centred on Lyriel, slightly below feet
    const float groundY = pos.y + 0.05f;
    const float segW    = 0.30f;
    const float segD    = 0.12f; // depth along Z
    const float padding = 0.05f;
    const int   segs    = m_numSegments > 0 ? m_numSegments : 1;
    const float total   = segs * segW + (segs - 1) * padding;
    const float startX  = pos.x - total * 0.5f;

    const float gaugePercent = getGaugePercent();
    const float filled       = gaugePercent * static_cast<float>(segs);

    const Vector3 colFilled = { 0.85f, 0.10f, 0.10f };
    const Vector3 colEmpty  = { 0.30f, 0.30f, 0.30f };

    for (int i = 0; i < segs; ++i)
    {
        const float x0 = startX + i * (segW + padding);
        const float x1 = x0 + segW;
        const float z0 = pos.z - segD * 0.5f;
        const float z1 = pos.z + segD * 0.5f;

        const float segFill = filled - static_cast<float>(i);
        const float ratio   = segFill < 0.0f ? 0.0f : (segFill > 1.0f ? 1.0f : segFill);
        const Vector3 col   = ratio > 0.0f ? colFilled : colEmpty;

        // Outline rectangle in XZ
        DebugDrawAPI::drawLine({ x0, groundY, z0 }, { x1, groundY, z0 }, col, 0, true);
        DebugDrawAPI::drawLine({ x0, groundY, z1 }, { x1, groundY, z1 }, col, 0, true);
        DebugDrawAPI::drawLine({ x0, groundY, z0 }, { x0, groundY, z1 }, col, 0, true);
        DebugDrawAPI::drawLine({ x1, groundY, z0 }, { x1, groundY, z1 }, col, 0, true);

        // Fill: diagonal cross-lines inside the filled portion of the segment
        if (ratio > 0.0f)
        {
            const float fillX = x0 + segW * ratio;
            DebugDrawAPI::drawLine({ x0,    groundY, z0 }, { fillX, groundY, z1 }, colFilled, 0, true);
            DebugDrawAPI::drawLine({ x0,    groundY, z1 }, { fillX, groundY, z0 }, colFilled, 0, true);
        }
    }

    // Decay timer arc around bar centre — white=grace, red=decaying
    if (m_everExploited && m_gauge > 0.0f)
    {
        const bool  inGrace = m_decayTimer <= m_gracePeriod;
        const float ratio   = inGrace
            ? 1.0f - (m_decayTimer / m_gracePeriod)
            : 0.0f;

        const Vector3 arcColor = inGrace ? Vector3{ 1.0f, 1.0f, 1.0f } : Vector3{ 1.0f, 0.2f, 0.2f };
        const float   arcR     = total * 0.5f + 0.2f;
        const int     totalSeg = 24;
        const int     fillSeg  = inGrace
            ? static_cast<int>(ratio * static_cast<float>(totalSeg))
            : totalSeg;
        constexpr float pi2    = 2.0f * 3.14159265f;
        const float     step   = pi2 / static_cast<float>(totalSeg);

        for (int i = 0; i < fillSeg; ++i)
        {
            const float   a0 = step * static_cast<float>(i);
            const float   a1 = a0 + step;
            const Vector3 p0 = { pos.x + cosf(a0) * arcR, groundY, pos.z + sinf(a0) * arcR };
            const Vector3 p1 = { pos.x + cosf(a1) * arcR, groundY, pos.z + sinf(a1) * arcR };
            DebugDrawAPI::drawLine(p0, p1, arcColor, 0, true);
        }
    }
}

IMPLEMENT_SCRIPT(ReaperGauge)

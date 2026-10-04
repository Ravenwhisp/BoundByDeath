#include "pch.h"
#include "ScreenFader.h"

#include "Transform2D.h"

IMPLEMENT_SCRIPT_FIELDS(ScreenFader,
    SERIALIZED_COMPONENT_REF(m_fadeImage, "Fade Image", ComponentType::TRANSFORM2D),
    SERIALIZED_FLOAT(m_initialAlpha, "Initial Alpha", 0.0f, 1.0f, 0.01f)
)

ScreenFader::ScreenFader(GameObject* owner)
    : Script(owner)
{
}

void ScreenFader::Start()
{
    m_fadeImageTransform2D = m_fadeImage.getReferencedComponent();

    if (m_fadeImageTransform2D == nullptr)
    {
        Debug::warn("ScreenFader on '%s' has no Fade Image assigned.", GameObjectAPI::getName(getOwner()));
        return;
    }

    setAlpha(m_initialAlpha);
}

void ScreenFader::Update()
{
    if (!m_isFading || m_fadeImageTransform2D == nullptr)
    {
        return;
    }

    m_timer += Time::getDeltaTime();

    const float normalizedTime = m_timer >= m_duration ? 1.0f : m_timer / m_duration;
    const float easedTime = MathAPI::smoothStep(0.0f, 1.0f, normalizedTime);

    m_currentAlpha = MathAPI::lerp(m_startAlpha, m_targetAlpha, easedTime);
    applyAlpha(m_currentAlpha);

    if (m_timer >= m_duration)
    {
        m_currentAlpha = m_targetAlpha;
        applyAlpha(m_currentAlpha);

        m_isFading = false;
        m_timer = 0.0f;
    }
}

void ScreenFader::fadeTo(float targetAlpha, float duration)
{
    if (m_fadeImageTransform2D == nullptr)
    {
        m_fadeImageTransform2D = m_fadeImage.getReferencedComponent();
    }

    if (m_fadeImageTransform2D == nullptr)
    {
        return;
    }

    m_startAlpha = Transform2DAPI::getAlpha(m_fadeImageTransform2D);
    m_currentAlpha = m_startAlpha;
    m_targetAlpha = targetAlpha;

    m_duration = duration;
    m_timer = 0.0f;

    if (m_duration <= 0.0f)
    {
        setAlpha(m_targetAlpha);
        return;
    }

    m_isFading = true;
}

void ScreenFader::setAlpha(float alpha)
{
    m_currentAlpha = alpha;
    m_startAlpha = alpha;
    m_targetAlpha = alpha;
    m_timer = 0.0f;
    m_isFading = false;

    applyAlpha(alpha);
}

void ScreenFader::applyAlpha(float alpha)
{
    if (m_fadeImageTransform2D == nullptr)
    {
        return;
    }

    Transform2DAPI::setAlpha(m_fadeImageTransform2D, alpha);
}

IMPLEMENT_SCRIPT(ScreenFader)

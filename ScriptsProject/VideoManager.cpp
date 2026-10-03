#include "pch.h"
#include "VideoManager.h"
#include "UISlider.h"

IMPLEMENT_SCRIPT_FIELDS(VideoManager,
    SERIALIZED_COMPONENT_REF(m_videoObject, "Video Object", ComponentType::TRANSFORM),
    SERIALIZED_COMPONENT_REF(m_skipSlider, "Skip Hold Slider", ComponentType::TRANSFORM),
    SERIALIZED_STRING(m_sceneToLoad, "Next Scene")
)

VideoManager::VideoManager(GameObject* owner)
    : Script(owner)
{
}

void VideoManager::Start()
{
    if (Transform* sliderTransform = m_skipSlider.getReferencedComponent())
    {
        GameObject* sliderOwner = ComponentAPI::getOwner(sliderTransform);
        m_skipSliderComponent = static_cast<UISlider*>(GameObjectAPI::getComponent(sliderOwner, ComponentType::UISLIDER));
    }

    GameObject* videoOwner = getOwner();
    if (Transform* videoObjectTransform = m_videoObject.getReferencedComponent())
    {
        videoOwner = ComponentAPI::getOwner(videoObjectTransform);
    }

    m_videoComponent = VideoAPI::getVideoComponent(videoOwner);

    if (m_videoComponent)
    {
        VideoAPI::play(m_videoComponent);
        m_started = true;
    }
}

void VideoManager::Update()
{
    if (!m_videoComponent)
    {
        return;
    }

    if (Input::isFaceButtonLeftPressed(0))
    {
        m_gamepadSkipHoldTime += Time::getDeltaTime();
    }
    else
    {
        m_gamepadSkipHoldTime = 0.0f;
    }

    if (m_skipSliderComponent)
    {
        const float holdProgress = m_gamepadSkipHoldTime >= 3.0f ? 1.0f : m_gamepadSkipHoldTime / 3.0f;
        SliderAPI::setFillAmount(m_skipSliderComponent, holdProgress);
    }

    const bool skipRequested = Input::isKeyDown(KeyCode::Escape) || m_gamepadSkipHoldTime >= 3.0f;
    const bool finished = m_started && !VideoAPI::isPlaying(m_videoComponent);

    if (skipRequested || finished)
    {
        VideoAPI::stop(m_videoComponent);

        if (!m_sceneToLoad.empty())
        {
            SceneAPI::requestSceneChange(m_sceneToLoad.c_str());
        }
    }
}

IMPLEMENT_SCRIPT(VideoManager)

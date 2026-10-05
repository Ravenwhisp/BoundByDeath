#include "pch.h"
#include "VideoManager.h"
#include "UISlider.h"
#include "Transform2D.h"


IMPLEMENT_SCRIPT_FIELDS(VideoManager,
    SERIALIZED_COMPONENT_REF(m_videoObject, "Video Object", ComponentType::TRANSFORM),
    SERIALIZED_COMPONENT_REF(m_skipSlider, "Skip Hold Slider", ComponentType::TRANSFORM),
    SERIALIZED_COMPONENT_REF(m_loadingImage, "Loading Image", ComponentType::TRANSFORM),
    SERIALIZED_COMPONENT_REF(m_skipContainer, "Skip Container", ComponentType::TRANSFORM),
    SERIALIZED_STRING(m_sceneToLoad, "Next Scene")
)

VideoManager::VideoManager(GameObject* owner) : Script(owner)
{
}

void VideoManager::Start()
{
    if (!m_sceneToLoad.empty())
    {
        m_asyncLoadStarted = SceneAPI::beginAsyncSceneLoad(m_sceneToLoad.c_str());
    }

    if (Transform* loadingImageTransform = m_loadingImage.getReferencedComponent())
    {
        GameObject* loadingImageOwner = ComponentAPI::getOwner(loadingImageTransform);
        m_loadingImageTransform = static_cast<Transform2D*>(GameObjectAPI::getComponent(loadingImageOwner, ComponentType::TRANSFORM2D));
    }

    if (Transform* skipContainerTransform = m_skipContainer.getReferencedComponent())
    {
        m_skipContainerOwner = ComponentAPI::getOwner(skipContainerTransform);
        m_skipContainerTransform = static_cast<Transform2D*>(GameObjectAPI::getComponent(m_skipContainerOwner, ComponentType::TRANSFORM2D));
        GameObjectAPI::setActive(m_skipContainerOwner, false);
    }

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
    const bool asyncReady = m_asyncLoadStarted && SceneAPI::isAsyncSceneLoadReady();
    const bool asyncLoading = m_asyncLoadStarted && SceneAPI::isAsyncSceneLoading();
    const bool asyncFailed = !asyncReady && !asyncLoading;

    if (asyncFailed && !m_asyncFailureLogged && !m_sceneToLoad.empty())
    {
        Debug::log("Async scene load unavailable for %s; using synchronous fallback.", m_sceneToLoad.c_str());
        m_asyncFailureLogged = true;
    }

    if (m_asyncTransitionPending && asyncFailed)
    {
        m_asyncTransitionPending = false;
        SceneAPI::requestSceneChange(m_sceneToLoad.c_str());
    }

    if (m_transitionRequested)
    {
        return;
    }

    if (!m_videoComponent)
    {
        return;
    }

    if (Input::isFaceButtonBottomPressed(0) && (asyncReady || asyncFailed))
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

    if (!m_skipAvailable && (asyncReady || asyncFailed))
    {
        m_skipAvailable = true;
        if (m_skipContainerOwner)
        {
            GameObjectAPI::setActive(m_skipContainerOwner, true);
        }
    }

    const bool skipRequested = m_skipAvailable && (Input::isKeyDown(KeyCode::Escape) || m_gamepadSkipHoldTime >= 3.0f);
    const bool finished = m_started && !VideoAPI::isPlaying(m_videoComponent);

    if (skipRequested || finished)
    {
        m_transitionRequested = true;
        VideoAPI::stop(m_videoComponent);

        if (m_loadingImageTransform)
        {
            Transform2DAPI::setAlpha(m_loadingImageTransform, 1.0f);
        }

        if (m_skipContainerTransform)
        {
            Transform2DAPI::setAlpha(m_skipContainerTransform, 0.0f);
        }

        if (!m_sceneToLoad.empty())
        {
            if ((asyncReady || asyncLoading) && SceneAPI::requestAsyncSceneChange())
            {
                m_asyncTransitionPending = true;
            }
            else
            {
                SceneAPI::requestSceneChange(m_sceneToLoad.c_str());
            }
        }
    }
}

IMPLEMENT_SCRIPT(VideoManager)

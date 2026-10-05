#pragma once

#include "ScriptAPI.h"

class ComponentVideo;
class UISlider;

class VideoManager : public Script
{
    DECLARE_SCRIPT(VideoManager)

public:
    explicit VideoManager(GameObject* owner);

    void Start() override;
    void Update() override;

    FieldList getExposedFields() const override;

public:
    ComponentRef<Transform> m_videoObject;
    ComponentRef<Transform> m_skipSlider;
    ComponentRef<Transform> m_loadingImage;
    ComponentRef<Transform> m_skipContainer;
    std::string m_sceneToLoad;

private:
    GameObject* m_skipContainerOwner = nullptr;
    ComponentVideo* m_videoComponent = nullptr;
    UISlider* m_skipSliderComponent = nullptr;
    Transform2D* m_loadingImageTransform = nullptr;
    Transform2D* m_skipContainerTransform = nullptr;
    bool m_started = false;
    bool m_asyncLoadStarted = false;
    bool m_skipAvailable = false;
    bool m_transitionRequested = false;
    bool m_asyncTransitionPending = false;
    bool m_asyncFailureLogged = false;
    float m_gamepadSkipHoldTime = 0.0f;
};

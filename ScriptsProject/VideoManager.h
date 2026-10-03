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
    std::string m_sceneToLoad;

private:
    ComponentVideo* m_videoComponent = nullptr;
    UISlider* m_skipSliderComponent = nullptr;
    bool m_started = false;
    float m_gamepadSkipHoldTime = 0.0f;
};

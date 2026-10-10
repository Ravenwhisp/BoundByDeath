#pragma once

#include "ScriptAPI.h"

class UISplashScreen : public Script
{
    DECLARE_SCRIPT(UISplashScreen)

public:
    explicit UISplashScreen(GameObject* owner);

    void Start() override;
    void Update() override;

    FieldList getExposedFields() const override;

public:
    ComponentRef<Transform2D> buttonGlow;
    ComponentRef<Transform2D> button;
    ComponentRef<Transform2D> logoGlow;
    ComponentRef<Transform2D> lyrielDeath;
    ComponentRef<Transform2D> particles1;
    ComponentRef<Transform2D> particles2;
    ComponentRef<Transform2D> particles3;
    std::string nextSceneName;

private:
    bool m_asyncLoadStarted = false;
    bool m_transitionRequested = false;
    bool m_asyncTransitionPending = false;

	float time = 0.0f;
	Transform2D* m_buttonGlow = nullptr;
	Transform2D* m_button = nullptr;
	Transform2D* m_logoGlow = nullptr;
	Transform2D* m_lyrielDeath = nullptr;
	Transform2D* m_particles1 = nullptr;
	Transform2D* m_particles2 = nullptr;
    Transform2D* m_particles3 = nullptr;

	bool isStarted = false;
    float startTime = 3.0f;
	float startTimer = 0.0f;
};

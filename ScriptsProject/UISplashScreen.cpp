#include "pch.h"
#include "UISplashScreen.h"
#include "Transform2D.h"
#include "PlayerGamepadBinding.h"

IMPLEMENT_SCRIPT_FIELDS(UISplashScreen,
    SERIALIZED_COMPONENT_REF(buttonGlow, "Button Glow", ComponentType::TRANSFORM2D),
	SERIALIZED_COMPONENT_REF(button, "Button", ComponentType::TRANSFORM2D),
	SERIALIZED_COMPONENT_REF(logoGlow, "Logo Glow", ComponentType::TRANSFORM2D),
    SERIALIZED_COMPONENT_REF(lyrielDeath, "Lyriel Death", ComponentType::TRANSFORM2D),
    SERIALIZED_COMPONENT_REF(particles1, "Particles 1", ComponentType::TRANSFORM2D),
    SERIALIZED_COMPONENT_REF(particles2, "Particles 2", ComponentType::TRANSFORM2D),
	SERIALIZED_COMPONENT_REF(particles3, "Particles 3", ComponentType::TRANSFORM2D),
	SERIALIZED_STRING(nextSceneName, "Next Scene Name")
)

UISplashScreen::UISplashScreen(GameObject* owner)
    : Script(owner)
{
}

void UISplashScreen::Start()
{
    if (!nextSceneName.empty())
    {
        m_asyncLoadStarted = SceneAPI::beginAsyncSceneLoad(nextSceneName.c_str());
    }

	m_buttonGlow = buttonGlow.getReferencedComponent();
	m_button = button.getReferencedComponent();
	m_logoGlow = logoGlow.getReferencedComponent();
	m_lyrielDeath = lyrielDeath.getReferencedComponent();
	m_particles1 = particles1.getReferencedComponent();
	m_particles2 = particles2.getReferencedComponent();
	m_particles3 = particles3.getReferencedComponent();

    if (m_particles3)
    {
		Transform2DAPI::setAlpha(m_particles3, 0.0f);
    }
}

void UISplashScreen::Update()
{
    const bool asyncAvailable = m_asyncLoadStarted && (SceneAPI::isAsyncSceneLoading() || SceneAPI::isAsyncSceneLoadReady());
    if (m_asyncTransitionPending && !asyncAvailable)
    {
        m_asyncTransitionPending = false;
        Debug::log("Async scene load failed for %s; using synchronous fallback.", nextSceneName.c_str());
        SceneAPI::requestSceneChange(nextSceneName.c_str());
    }

    if (!m_transitionRequested && (Input::isFaceButtonLeftJustPressed() ||
        Input::isFaceButtonRightJustPressed() ||
        Input::isFaceButtonTopJustPressed() ||
        Input::isFaceButtonBottomJustPressed() ||
		Input::isPauseJustPressed() ||
        Input::isLeftShoulderJustPressed() ||
        //Input::isRightShoulderJustPressed() || (Mouse Click)
        Input::isLeftTriggerJustPressed() ||
        //Input::isRightTriggerJustPressed() || (Mouse Right Click)
        Input::isFaceButtonLeftJustPressed(1) ||
        Input::isFaceButtonRightJustPressed(1) ||
        Input::isFaceButtonTopJustPressed(1) ||
        Input::isFaceButtonBottomJustPressed(1) ||
        Input::isPauseJustPressed(1) ||
        Input::isLeftShoulderJustPressed(1) ||
        Input::isRightShoulderJustPressed(1) ||
        Input::isLeftTriggerJustPressed(1) ||
        Input::isRightTriggerJustPressed(1)
        ))
    {
		isStarted = true;
		startTimer = startTime;
    }

    if (!nextSceneName.empty() && isStarted && startTimer <= 0.0f)
    {
        m_transitionRequested = true;
        if (asyncAvailable && SceneAPI::requestAsyncSceneChange())
        {
            m_asyncTransitionPending = true;
        }
        else
        {
            Debug::log("Async scene load unavailable for %s; using synchronous fallback.", nextSceneName.c_str());
            SceneAPI::requestSceneChange(nextSceneName.c_str());
        }
    }
    
    if (isStarted && startTimer > 0.0f)
    {
        startTimer -= Time::getDeltaTime();
		const float t = startTimer / startTime;
		Transform2DAPI::setAlpha(m_particles3, 1.0f - t);
		Transform2DAPI::setAlpha(m_button, t * 2.0f - 1.0f);
    }

	time += Time::getDeltaTime();

    if (m_buttonGlow && !isStarted)
    {
		Transform2DAPI::setAlpha(m_buttonGlow, std::abs(std::sin(time * 2.0f)));
    }
    else
    {
		const float alpha = MathAPI::moveTowards(Transform2DAPI::getAlpha(m_buttonGlow), 0.0f, Time::getDeltaTime() * 2.0f);
		Transform2DAPI::setAlpha(m_buttonGlow, alpha);
    }
    if (m_logoGlow)
    {
        const float t = (std::sin(time * 2.4f) + 1.0f) * 0.5f;
        const float alpha = MathAPI::evaluateEasing(MathAPI::EasingType::EaseInQuad, t);
        Transform2DAPI::setAlpha(m_logoGlow, alpha);
    }
    if (m_lyrielDeath)
    {
		// Figure-8 movement
        const Vector2 offset = Vector2(
            std::sin(time * 0.8f) * 8.0f, //w-speed // offset // width
			std::sin(time * 0.8f * 2.0f) * 4.0f //h-speed // offset // height
        );
        Transform2DAPI::setPosition(m_lyrielDeath, offset);

        // Breathing scalar
        const float t = (std::sin(time * 1.2f /*speed*/) + 1.0f) * 0.5f;
        const float scale = MathAPI::lerp(0.97f, 1.03f, t);
        Transform2DAPI::setScale(m_lyrielDeath, Vector2(scale, scale));
    }
    if (m_particles1)
    {
        const Vector2 offset = Vector2(
            std::sin(time * 0.3f) * 15.0f,
            std::sin(time * 0.3f * 2.0f) * 20.0f
        );
        Transform2DAPI::setPosition(m_particles2, offset);

        const float t = (std::sin(time * 1.2f) + 1.0f) * 0.5f;
        const float alpha = MathAPI::lerp(0.4f, 0.85f, t);
		Transform2DAPI::setAlpha(m_particles1, alpha);
    }
    if (m_particles2)
    {
        const Vector2 offset = Vector2(
            std::sin(time * 0.3f + 0.4f) * 3.0f,
            std::sin(time * 0.3f * 2.0f + 0.6f) * 5.0f
        );
        Transform2DAPI::setPosition(m_particles1, offset);

        const float t = (std::sin(time * 0.8f + 0.2f) + 1.0f) * 0.5f;
        const float alpha = MathAPI::lerp(0.35f, 0.55f, t);
        Transform2DAPI::setAlpha(m_particles2, alpha);
    }
}

IMPLEMENT_SCRIPT(UISplashScreen)

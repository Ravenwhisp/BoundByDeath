#include "pch.h"
#include "PopUpController.h"

#include "PopUpEvent.h"
#include "PlayerController.h"

PopUpController::PopUpController(GameObject* owner)
    : Script(owner)
{
}

void PopUpController::Start()
{
    findPlayerControllers();
}

void PopUpController::Update()
{
    if (!m_isShowingPopUp)
    {
        return;
    }

    const float dt = Time::getDeltaTime();

    switch (m_state)
    {
    case PopUpState::FadingIn:
        updateFadingIn(dt);
        break;

    case PopUpState::WaitingForConfirmation:
        updateWaitingForConfirmation();
        break;

    case PopUpState::FadingOut:
        updateFadingOut(dt);
        break;

    case PopUpState::None:
    default:
        break;
    }
}

void PopUpController::startPopUp(PopUpEvent* event)
{
    if (m_isShowingPopUp)
    {
        return;
    }

    if (event == nullptr)
    {
        return;
    }

    m_currentEvent = event;
    m_currentPopUpImage = nullptr;

    m_isShowingPopUp = true;
    m_state = PopUpState::FadingIn;

    m_timer = 0.0f;
    m_currentAlpha = 0.0f;

    m_player1Confirmed = false;
    m_player2Confirmed = false;

    setPlayersGameplayInputLocked(true);
    setPopUpAlpha(0.0f);
}

void PopUpController::updateFadingIn(float dt)
{
    const float duration = 0.25f;

    m_timer += dt;

    const float normalizedTime = m_timer >= duration ? 1.0f : m_timer / duration;
    const float alpha = MathAPI::smoothStep(0.0f, 1.0f, normalizedTime);

    m_currentAlpha = alpha;
    setPopUpAlpha(m_currentAlpha);

    if (m_timer >= duration)
    {
        m_currentAlpha = 1.0f;
        setPopUpAlpha(m_currentAlpha);

        m_state = PopUpState::WaitingForConfirmation;
        m_timer = 0.0f;
    }
}

void PopUpController::updateWaitingForConfirmation()
{
    if (Input::isFaceButtonBottomJustPressed(0))
    {
        m_player1Confirmed = true;
    }

    if (Input::isFaceButtonBottomJustPressed(1))
    {
        m_player2Confirmed = true;
    }

    if (!m_player1Confirmed || !m_player2Confirmed)
    {
        return;
    }

    m_state = PopUpState::FadingOut;
    m_timer = 0.0f;
}

void PopUpController::updateFadingOut(float dt)
{
    const float duration = 0.25f;

    m_timer += dt;

    const float normalizedTime = m_timer >= duration ? 1.0f : m_timer / duration;
    const float alpha = MathAPI::smoothStep(0.0f, 1.0f, normalizedTime);

    m_currentAlpha = MathAPI::lerp(1.0f, 0.0f, alpha);
    setPopUpAlpha(m_currentAlpha);

    if (m_timer >= duration)
    {
        m_currentAlpha = 0.0f;
        setPopUpAlpha(m_currentAlpha);

        finishPopUp();
    }
}

void PopUpController::finishPopUp()
{
    setPlayersGameplayInputLocked(false);

    m_currentEvent = nullptr;
    m_currentPopUpImage = nullptr;

    m_state = PopUpState::None;
    m_isShowingPopUp = false;

    m_player1Confirmed = false;
    m_player2Confirmed = false;

    m_timer = 0.0f;
    m_currentAlpha = 0.0f;
}

void PopUpController::findPlayerControllers()
{
    m_playerControllers.clear();

    const std::vector<GameObject*> players = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER, true);

    for (GameObject* player : players)
    {
        PlayerController* playerController = GameObjectAPI::findScript<PlayerController>(player);

        if (playerController == nullptr)
        {
            Debug::warn("PopUpController could not find PlayerController on player '%s'.", GameObjectAPI::getName(player));
            continue;
        }

        m_playerControllers.push_back(playerController);
    }
}

void PopUpController::setPlayersGameplayInputLocked(bool locked)
{
    for (PlayerController* playerController : m_playerControllers)
    {
        if (playerController == nullptr)
        {
            continue;
        }

        playerController->setGameplayInputLocked(locked);
    }
}

void PopUpController::setPopUpAlpha(float alpha)
{
    if (m_currentPopUpImage == nullptr)
    {
        return;
    }

    Transform2DAPI::setAlpha(m_currentPopUpImage, alpha);
}

IMPLEMENT_SCRIPT(PopUpController)
#include "pch.h"
#include "TutorialUIController.h"

#include "TutorialUIEvent.h"
#include "PlayerController.h"
#include "Damageable.h"

TutorialUIController::TutorialUIController(GameObject* owner)
    : Script(owner)
{
}

void TutorialUIController::Start()
{
    findPlayerControllers();
}

void TutorialUIController::Update()
{
    if (!m_isShowingTutorialUI)
    {
        return;
    }

    const float dt = Time::getDeltaTime();

    switch (m_state)
    {
    case TutorialUIState::FadingIn:
        updateFadingIn(dt);
        break;

    case TutorialUIState::WaitingForConfirmation:
        updateWaitingForConfirmation();
        break;

    case TutorialUIState::FadingOut:
        updateFadingOut(dt);
        break;

    case TutorialUIState::None:
    default:
        break;
    }
}

void TutorialUIController::startTutorialUI(TutorialUIEvent* event)
{
    if (m_isShowingTutorialUI)
    {
        return;
    }

    if (event == nullptr)
    {
        return;
    }

    m_currentEvent = event;
    m_currentTutorialUIImage = event->getTutorialUIImageTransform2D();

    m_isShowingTutorialUI = true;
    m_state = TutorialUIState::FadingIn;

    m_timer = 0.0f;
    m_currentAlpha = 0.0f;

    m_player1Confirmed = false;
    m_player2Confirmed = false;

    setPlayersGameplayInputLocked(true);
    setPlayersInvulnerable(true);
    setTutorialUIAlpha(0.0f);
}

void TutorialUIController::updateFadingIn(float dt)
{
    const float duration = m_currentEvent->getShowDuration();

    m_timer += dt;

    const float normalizedTime = m_timer >= duration ? 1.0f : m_timer / duration;
    const float alpha = MathAPI::smoothStep(0.0f, 1.0f, normalizedTime);

    m_currentAlpha = alpha;
    setTutorialUIAlpha(m_currentAlpha);

    if (m_timer >= duration)
    {
        m_currentAlpha = 1.0f;
        setTutorialUIAlpha(m_currentAlpha);

        m_state = TutorialUIState::WaitingForConfirmation;
        m_timer = 0.0f;
    }
}

void TutorialUIController::updateWaitingForConfirmation()
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

    m_state = TutorialUIState::FadingOut;
    m_timer = 0.0f;
}

void TutorialUIController::updateFadingOut(float dt)
{
    const float duration = m_currentEvent->getHideDuration();

    m_timer += dt;

    const float normalizedTime = m_timer >= duration ? 1.0f : m_timer / duration;
    const float alpha = MathAPI::smoothStep(0.0f, 1.0f, normalizedTime);

    m_currentAlpha = MathAPI::lerp(1.0f, 0.0f, alpha);
    setTutorialUIAlpha(m_currentAlpha);

    if (m_timer >= duration)
    {
        m_currentAlpha = 0.0f;
        setTutorialUIAlpha(m_currentAlpha);

        finishTutorialUI();
    }
}

void TutorialUIController::finishTutorialUI()
{
    setPlayersGameplayInputLocked(false);
    setPlayersInvulnerable(false);

    m_currentEvent = nullptr;
    m_currentTutorialUIImage = nullptr;

    m_state = TutorialUIState::None;
    m_isShowingTutorialUI = false;

    m_player1Confirmed = false;
    m_player2Confirmed = false;

    m_timer = 0.0f;
    m_currentAlpha = 0.0f;
}

void TutorialUIController::findPlayerControllers()
{
    m_playerControllers.clear();
    m_playerDamageables.clear();

    const std::vector<GameObject*> players = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER, true);

    for (GameObject* player : players)
    {
        PlayerController* playerController = GameObjectAPI::findScript<PlayerController>(player);
        if (playerController == nullptr)
        {
            Debug::warn("TutorialUIController could not find PlayerController on player '%s'.", GameObjectAPI::getName(player));
        }
        else
        {
            m_playerControllers.push_back(playerController);
        }

        Damageable* damageable = GameObjectAPI::findScript<Damageable>(player);
        if (damageable == nullptr)
        {
            Debug::warn("TutorialUIController could not find Damageable on player '%s'.", GameObjectAPI::getName(player));
        }
        else
        {
            m_playerDamageables.push_back(damageable);
        }
    }
}

void TutorialUIController::setPlayersGameplayInputLocked(bool locked)
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

void TutorialUIController::setPlayersInvulnerable(bool invulnerable)
{
    for (Damageable* damageable : m_playerDamageables)
    {
        if (damageable == nullptr)
        {
            continue;
        }

        damageable->setInvulnerable(invulnerable);
    }
}

void TutorialUIController::setTutorialUIAlpha(float alpha)
{
    if (m_currentTutorialUIImage == nullptr)
    {
        return;
    }

    Transform2DAPI::setAlpha(m_currentTutorialUIImage, alpha);
}

IMPLEMENT_SCRIPT(TutorialUIController)
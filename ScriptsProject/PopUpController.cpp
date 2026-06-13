#include "pch.h"
#include "PopUpController.h"

#include "PopUpEvent.h"
#include "PlayerController.h"
#include "Damageable.h"

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
    case PopUpState::Showing:
        updateShowing(dt);
        break;

    case PopUpState::Waiting:
        updateWaiting();
        break;

    case PopUpState::Hiding:
        updateHiding(dt);
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
    m_currentImageIndex = 0;

    hideAllPopUpImages();

    if (!setCurrentPopUpImage(m_currentImageIndex))
    {
        m_currentEvent = nullptr;
        return;
    }

    m_isShowingPopUp = true;
    m_state = PopUpState::Showing;

    m_timer = 0.0f;
    m_currentAlpha = 0.0f;

    m_player1Confirmed = false;
    m_player2Confirmed = false;
    m_objectiveCompleted = false;

    if (m_currentEvent->shouldLockGameplay())
    {
        setPlayersGameplayInputLocked(true);
        setPlayersInvulnerable(true);
    }

    prepareShowTransition();
}

void PopUpController::notifyObjectiveCompleted()
{
    if (!m_isShowingPopUp)
    {
        return;
    }

    m_objectiveCompleted = true;
}

void PopUpController::updateShowing(float dt)
{
    const float duration = m_currentEvent->getShowDuration();

    m_timer += dt;

    const float normalizedTime = m_timer >= duration ? 1.0f : m_timer / duration;
    const float alpha = MathAPI::smoothStep(0.0f, 1.0f, normalizedTime);

    updateShowTransition(alpha);

    if (m_timer >= duration)
    {
        updateShowTransition(1.0f);

        m_state = PopUpState::Waiting;
        m_timer = 0.0f;
    }
}

void PopUpController::updateWaiting()
{
    if (m_currentEvent == nullptr)
    {
        return;
    }

    switch (m_currentEvent->getCloseMode())
    {
    case PopUpCloseMode::BothPlayersConfirm:
        if (Input::isFaceButtonBottomJustPressed(0))
        {
            m_player1Confirmed = true;
        }

        if (Input::isFaceButtonBottomJustPressed(1))
        {
            m_player2Confirmed = true;
        }

        if (m_player1Confirmed && m_player2Confirmed)
        {
            prepareHideTransition();
            m_state = PopUpState::Hiding;
            m_timer = 0.0f;
        }
        break;

    case PopUpCloseMode::ObjectiveCompleted:
        if (m_objectiveCompleted)
        {
            prepareHideTransition();
            m_state = PopUpState::Hiding;
            m_timer = 0.0f;
        }
        break;

    default:
        break;
    }
}

void PopUpController::updateHiding(float dt)
{
    const float duration = m_currentEvent->getHideDuration();

    m_timer += dt;

    const float normalizedTime = m_timer >= duration ? 1.0f : m_timer / duration;
    const float alpha = MathAPI::smoothStep(0.0f, 1.0f, normalizedTime);

    updateHideTransition(alpha);

    if (m_timer >= duration)
    {
        updateHideTransition(1.0f);

        const int nextImageIndex = m_currentImageIndex + 1;

        if (nextImageIndex < m_currentEvent->getPopUpImageCount())
        {
            m_currentImageIndex = nextImageIndex;

            m_player1Confirmed = false;
            m_player2Confirmed = false;
            m_objectiveCompleted = false;

            if (setCurrentPopUpImage(m_currentImageIndex))
            {
                m_state = PopUpState::Showing;
                m_timer = 0.0f;
                m_currentAlpha = 0.0f;

                prepareShowTransition();
                return;
            }
        }

        finishPopUp();
    }
}

void PopUpController::prepareShowTransition()
{
    if (m_currentPopUpImage == nullptr || m_currentEvent == nullptr)
    {
        return;
    }

    m_visiblePosition = Transform2DAPI::getPosition(m_currentPopUpImage);
    m_hiddenPosition = calculateHiddenPosition();

    switch (m_currentEvent->getTransitionType())
    {
    case PopUpTransitionType::Fade:
        setPopUpAlpha(0.0f);
        break;

    case PopUpTransitionType::SlideFromLeft:
    case PopUpTransitionType::SlideFromRight:
        setPopUpAlpha(1.0f);
        setPopUpPosition(m_hiddenPosition);
        break;

    default:
        setPopUpAlpha(0.0f);
        break;
    }
}

void PopUpController::prepareHideTransition()
{
    if (m_currentPopUpImage == nullptr || m_currentEvent == nullptr)
    {
        return;
    }

    m_visiblePosition = Transform2DAPI::getPosition(m_currentPopUpImage);
    m_hiddenPosition = calculateHiddenPosition();

    switch (m_currentEvent->getTransitionType())
    {
    case PopUpTransitionType::Fade:
        setPopUpAlpha(1.0f);
        break;

    case PopUpTransitionType::SlideFromLeft:
    case PopUpTransitionType::SlideFromRight:
        setPopUpAlpha(1.0f);
        setPopUpPosition(m_visiblePosition);
        break;

    default:
        setPopUpAlpha(1.0f);
        break;
    }
}

void PopUpController::updateShowTransition(float alpha)
{
    if (m_currentEvent == nullptr || m_currentPopUpImage == nullptr)
    {
        return;
    }

    switch (m_currentEvent->getTransitionType())
    {
    case PopUpTransitionType::Fade:
        m_currentAlpha = alpha;
        setPopUpAlpha(m_currentAlpha);
        break;

    case PopUpTransitionType::SlideFromLeft:
    case PopUpTransitionType::SlideFromRight:
    {
        const Vector2 position = MathAPI::lerp(m_hiddenPosition, m_visiblePosition, alpha);
        setPopUpPosition(position);
        break;
    }

    default:
        m_currentAlpha = alpha;
        setPopUpAlpha(m_currentAlpha);
        break;
    }
}

void PopUpController::updateHideTransition(float alpha)
{
    if (m_currentEvent == nullptr || m_currentPopUpImage == nullptr)
    {
        return;
    }

    switch (m_currentEvent->getTransitionType())
    {
    case PopUpTransitionType::Fade:
        m_currentAlpha = MathAPI::lerp(1.0f, 0.0f, alpha);
        setPopUpAlpha(m_currentAlpha);
        break;

    case PopUpTransitionType::SlideFromLeft:
    case PopUpTransitionType::SlideFromRight:
    {
        const Vector2 position = MathAPI::lerp(m_visiblePosition, m_hiddenPosition, alpha);
        setPopUpPosition(position);
        break;
    }

    default:
        m_currentAlpha = MathAPI::lerp(1.0f, 0.0f, alpha);
        setPopUpAlpha(m_currentAlpha);
        break;
    }
}

bool PopUpController::setCurrentPopUpImage(int index)
{
    if (m_currentEvent == nullptr)
    {
        return false;
    }

    m_currentPopUpImage = m_currentEvent->getPopUpImageTransform2D(index);

    if (m_currentPopUpImage == nullptr)
    {
        Debug::warn("PopUpController could not set PopUpI Image at index %d.", index);
        return false;
    }

    return true;
}

void PopUpController::hideAllPopUpImages()
{
    if (m_currentEvent == nullptr)
    {
        return;
    }

    const int imageCount = m_currentEvent->getPopUpImageCount();

    for (int i = 0; i < imageCount; ++i)
    {
        Transform2D* image = m_currentEvent->getPopUpImageTransform2D(i);

        if (image == nullptr)
        {
            continue;
        }

        Transform2DAPI::setAlpha(image, 0.0f);
    }
}

void PopUpController::finishPopUp()
{
    if (m_currentEvent->shouldLockGameplay())
    {
        setPlayersGameplayInputLocked(false);
        setPlayersInvulnerable(false);
    }

    m_currentEvent = nullptr;
    m_currentPopUpImage = nullptr;
    m_currentImageIndex = 0;

    m_state = PopUpState::None;
    m_isShowingPopUp = false;

    m_player1Confirmed = false;
    m_player2Confirmed = false;
    m_objectiveCompleted = false;

    m_timer = 0.0f;
    m_currentAlpha = 0.0f;
}

void PopUpController::findPlayerControllers()
{
    m_playerControllers.clear();
    m_playerDamageables.clear();

    const std::vector<GameObject*> players = SceneAPI::findAllGameObjectsByTag(Tag::PLAYER, true);

    for (GameObject* player : players)
    {
        PlayerController* playerController = GameObjectAPI::findScript<PlayerController>(player);
        if (playerController == nullptr)
        {
            Debug::warn("PopUpController could not find PlayerController on player '%s'.", GameObjectAPI::getName(player));
        }
        else
        {
            m_playerControllers.push_back(playerController);
        }

        Damageable* damageable = GameObjectAPI::findScript<Damageable>(player);
        if (damageable == nullptr)
        {
            Debug::warn("PopUpController could not find Damageable on player '%s'.", GameObjectAPI::getName(player));
        }
        else
        {
            m_playerDamageables.push_back(damageable);
        }
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

void PopUpController::setPlayersInvulnerable(bool invulnerable)
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

void PopUpController::setPopUpAlpha(float alpha)
{
    if (m_currentPopUpImage == nullptr)
    {
        return;
    }

    Transform2DAPI::setAlpha(m_currentPopUpImage, alpha);
}

void PopUpController::setPopUpPosition(const Vector2& position)
{
    if (m_currentPopUpImage == nullptr)
    {
        return;
    }

    Transform2DAPI::setPosition(m_currentPopUpImage, position);
}

Vector2 PopUpController::calculateHiddenPosition() const
{
    if (m_currentEvent == nullptr)
    {
        return m_visiblePosition;
    }

    switch (m_currentEvent->getTransitionType())
    {
    case PopUpTransitionType::SlideFromLeft:
        return Vector2(m_visiblePosition.x - m_slideOffset, m_visiblePosition.y);

    case PopUpTransitionType::SlideFromRight:
        return Vector2(m_visiblePosition.x + m_slideOffset, m_visiblePosition.y);

    case PopUpTransitionType::Fade:
    default:
        return m_visiblePosition;
    }
}

IMPLEMENT_SCRIPT(PopUpController)
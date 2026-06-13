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
    case TutorialUIState::Showing:
        updateShowing(dt);
        break;

    case TutorialUIState::Waiting:
        updateWaiting();
        break;

    case TutorialUIState::Hiding:
        updateHiding(dt);
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
    m_currentImageIndex = 0;

    hideAllTutorialUIImages();

    if (!setCurrentTutorialUIImage(m_currentImageIndex))
    {
        m_currentEvent = nullptr;
        return;
    }

    m_isShowingTutorialUI = true;
    m_state = TutorialUIState::Showing;

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

void TutorialUIController::notifyObjectiveCompleted()
{
    if (!m_isShowingTutorialUI)
    {
        return;
    }

    m_objectiveCompleted = true;
}

void TutorialUIController::updateShowing(float dt)
{
    const float duration = m_currentEvent->getShowDuration();

    m_timer += dt;

    const float normalizedTime = m_timer >= duration ? 1.0f : m_timer / duration;
    const float alpha = MathAPI::smoothStep(0.0f, 1.0f, normalizedTime);

    updateShowTransition(alpha);

    if (m_timer >= duration)
    {
        updateShowTransition(1.0f);

        m_state = TutorialUIState::Waiting;
        m_timer = 0.0f;
    }
}

void TutorialUIController::updateWaiting()
{
    if (m_currentEvent == nullptr)
    {
        return;
    }

    switch (m_currentEvent->getCloseMode())
    {
    case TutorialUICloseMode::BothPlayersConfirm:
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
            m_state = TutorialUIState::Hiding;
            m_timer = 0.0f;
        }
        break;

    case TutorialUICloseMode::ObjectiveCompleted:
        if (m_objectiveCompleted)
        {
            prepareHideTransition();
            m_state = TutorialUIState::Hiding;
            m_timer = 0.0f;
        }
        break;

    default:
        break;
    }
}

void TutorialUIController::updateHiding(float dt)
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

        if (nextImageIndex < m_currentEvent->getTutorialUIImageCount())
        {
            m_currentImageIndex = nextImageIndex;

            m_player1Confirmed = false;
            m_player2Confirmed = false;
            m_objectiveCompleted = false;

            if (setCurrentTutorialUIImage(m_currentImageIndex))
            {
                m_state = TutorialUIState::Showing;
                m_timer = 0.0f;
                m_currentAlpha = 0.0f;

                prepareShowTransition();
                return;
            }
        }

        finishTutorialUI();
    }
}

void TutorialUIController::prepareShowTransition()
{
    if (m_currentTutorialUIImage == nullptr || m_currentEvent == nullptr)
    {
        return;
    }

    m_visiblePosition = Transform2DAPI::getPosition(m_currentTutorialUIImage);
    m_hiddenPosition = calculateHiddenPosition();

    switch (m_currentEvent->getTransitionType())
    {
    case TutorialUITransitionType::Fade:
        setTutorialUIAlpha(0.0f);
        break;

    case TutorialUITransitionType::SlideFromLeft:
    case TutorialUITransitionType::SlideFromRight:
        setTutorialUIAlpha(1.0f);
        setTutorialUIPosition(m_hiddenPosition);
        break;

    default:
        setTutorialUIAlpha(0.0f);
        break;
    }
}

void TutorialUIController::prepareHideTransition()
{
    if (m_currentTutorialUIImage == nullptr || m_currentEvent == nullptr)
    {
        return;
    }

    m_visiblePosition = Transform2DAPI::getPosition(m_currentTutorialUIImage);
    m_hiddenPosition = calculateHiddenPosition();

    switch (m_currentEvent->getTransitionType())
    {
    case TutorialUITransitionType::Fade:
        setTutorialUIAlpha(1.0f);
        break;

    case TutorialUITransitionType::SlideFromLeft:
    case TutorialUITransitionType::SlideFromRight:
        setTutorialUIAlpha(1.0f);
        setTutorialUIPosition(m_visiblePosition);
        break;

    default:
        setTutorialUIAlpha(1.0f);
        break;
    }
}

void TutorialUIController::updateShowTransition(float alpha)
{
    if (m_currentEvent == nullptr || m_currentTutorialUIImage == nullptr)
    {
        return;
    }

    switch (m_currentEvent->getTransitionType())
    {
    case TutorialUITransitionType::Fade:
        m_currentAlpha = alpha;
        setTutorialUIAlpha(m_currentAlpha);
        break;

    case TutorialUITransitionType::SlideFromLeft:
    case TutorialUITransitionType::SlideFromRight:
    {
        const Vector2 position = MathAPI::lerp(m_hiddenPosition, m_visiblePosition, alpha);
        setTutorialUIPosition(position);
        break;
    }

    default:
        m_currentAlpha = alpha;
        setTutorialUIAlpha(m_currentAlpha);
        break;
    }
}

void TutorialUIController::updateHideTransition(float alpha)
{
    if (m_currentEvent == nullptr || m_currentTutorialUIImage == nullptr)
    {
        return;
    }

    switch (m_currentEvent->getTransitionType())
    {
    case TutorialUITransitionType::Fade:
        m_currentAlpha = MathAPI::lerp(1.0f, 0.0f, alpha);
        setTutorialUIAlpha(m_currentAlpha);
        break;

    case TutorialUITransitionType::SlideFromLeft:
    case TutorialUITransitionType::SlideFromRight:
    {
        const Vector2 position = MathAPI::lerp(m_visiblePosition, m_hiddenPosition, alpha);
        setTutorialUIPosition(position);
        break;
    }

    default:
        m_currentAlpha = MathAPI::lerp(1.0f, 0.0f, alpha);
        setTutorialUIAlpha(m_currentAlpha);
        break;
    }
}

bool TutorialUIController::setCurrentTutorialUIImage(int index)
{
    if (m_currentEvent == nullptr)
    {
        return false;
    }

    m_currentTutorialUIImage = m_currentEvent->getTutorialUIImageTransform2D(index);

    if (m_currentTutorialUIImage == nullptr)
    {
        Debug::warn("TutorialUIController could not set Tutorial UI Image at index %d.", index);
        return false;
    }

    return true;
}

void TutorialUIController::hideAllTutorialUIImages()
{
    if (m_currentEvent == nullptr)
    {
        return;
    }

    const int imageCount = m_currentEvent->getTutorialUIImageCount();

    for (int i = 0; i < imageCount; ++i)
    {
        Transform2D* image = m_currentEvent->getTutorialUIImageTransform2D(i);

        if (image == nullptr)
        {
            continue;
        }

        Transform2DAPI::setAlpha(image, 0.0f);
    }
}

void TutorialUIController::finishTutorialUI()
{
    if (m_currentEvent->shouldLockGameplay())
    {
        setPlayersGameplayInputLocked(false);
        setPlayersInvulnerable(false);
    }

    m_currentEvent = nullptr;
    m_currentTutorialUIImage = nullptr;
    m_currentImageIndex = 0;

    m_state = TutorialUIState::None;
    m_isShowingTutorialUI = false;

    m_player1Confirmed = false;
    m_player2Confirmed = false;
    m_objectiveCompleted = false;

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

void TutorialUIController::setTutorialUIPosition(const Vector2& position)
{
    if (m_currentTutorialUIImage == nullptr)
    {
        return;
    }

    Transform2DAPI::setPosition(m_currentTutorialUIImage, position);
}

Vector2 TutorialUIController::calculateHiddenPosition() const
{
    if (m_currentEvent == nullptr)
    {
        return m_visiblePosition;
    }

    switch (m_currentEvent->getTransitionType())
    {
    case TutorialUITransitionType::SlideFromLeft:
        return Vector2(m_visiblePosition.x - m_slideOffset, m_visiblePosition.y);

    case TutorialUITransitionType::SlideFromRight:
        return Vector2(m_visiblePosition.x + m_slideOffset, m_visiblePosition.y);

    case TutorialUITransitionType::Fade:
    default:
        return m_visiblePosition;
    }
}

IMPLEMENT_SCRIPT(TutorialUIController)
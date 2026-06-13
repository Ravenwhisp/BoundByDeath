#pragma once

#include "ScriptAPI.h"

class TutorialUIEvent;
class PlayerController;
class Damageable;
class Transform2D;

class TutorialUIController : public Script
{
    DECLARE_SCRIPT(TutorialUIController)

public:
    explicit TutorialUIController(GameObject* owner);

    void Start() override;
    void Update() override;

    void startTutorialUI(TutorialUIEvent* event);
    void notifyObjectiveCompleted();

    bool isShowingTutorialUI() const { return m_isShowingTutorialUI; }

private:
    enum class TutorialUIState
    {
        None,
        Showing,
        Waiting,
        Hiding
    };

private:
    void updateShowing(float dt);
    void updateWaiting();
    void updateHiding(float dt);

    void prepareShowTransition();
    void prepareHideTransition();

    void updateShowTransition(float alpha);
    void updateHideTransition(float alpha);

    bool setCurrentTutorialUIImage(int index);
    void hideAllTutorialUIImages();

    void finishTutorialUI();

    void findPlayerControllers();
    void setPlayersGameplayInputLocked(bool locked);
    void setPlayersInvulnerable(bool invulnerable);

    void setTutorialUIAlpha(float alpha);
    void setTutorialUIPosition(const Vector2& position);
    Vector2 calculateHiddenPosition() const;

private:
    TutorialUIEvent* m_currentEvent = nullptr;
    Transform2D* m_currentTutorialUIImage = nullptr;

    int m_currentImageIndex = 0;

    std::vector<PlayerController*> m_playerControllers;
    std::vector<Damageable*> m_playerDamageables;

    TutorialUIState m_state = TutorialUIState::None;

    bool m_isShowingTutorialUI = false;

    bool m_player1Confirmed = false;
    bool m_player2Confirmed = false;
    bool m_objectiveCompleted = false;

    float m_timer = 0.0f;
    float m_currentAlpha = 0.0f;

    Vector2 m_visiblePosition = Vector2(0.0f, 0.0f);
    Vector2 m_hiddenPosition = Vector2(0.0f, 0.0f);

    float m_slideOffset = 600.0f;
};
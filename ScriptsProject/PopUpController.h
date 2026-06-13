#pragma once

#include "ScriptAPI.h"

class PopUpEvent;
class PlayerController;
class Damageable;
class Transform2D;

class PopUpController : public Script
{
    DECLARE_SCRIPT(PopUpController)

public:
    explicit PopUpController(GameObject* owner);

    void Start() override;
    void Update() override;

    void startPopUp(PopUpEvent* event);
    void notifyObjectiveCompleted();

    bool isShowingPopUp() const { return m_isShowingPopUp; }

private:
    enum class PopUpState
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

    bool setCurrentPopUpImage(int index);
    void hideAllPopUpImages();

    void finishPopUp();

    void findPlayerControllers();
    void setPlayersGameplayInputLocked(bool locked);
    void setPlayersInvulnerable(bool invulnerable);

    void setPopUpAlpha(float alpha);
    void setPopUpPosition(const Vector2& position);
    Vector2 calculateHiddenPosition() const;

private:
    PopUpEvent* m_currentEvent = nullptr;
    Transform2D* m_currentPopUpImage = nullptr;

    int m_currentImageIndex = 0;

    std::vector<PlayerController*> m_playerControllers;
    std::vector<Damageable*> m_playerDamageables;

    PopUpState m_state = PopUpState::None;

    bool m_isShowingPopUp = false;

    bool m_player1Confirmed = false;
    bool m_player2Confirmed = false;
    bool m_objectiveCompleted = false;

    float m_timer = 0.0f;
    float m_currentAlpha = 0.0f;

    Vector2 m_visiblePosition = Vector2(0.0f, 0.0f);
    Vector2 m_hiddenPosition = Vector2(0.0f, 0.0f);

    float m_slideOffset = 600.0f;
};
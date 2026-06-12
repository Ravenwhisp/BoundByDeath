#pragma once

#include "ScriptAPI.h"

class PopUpEvent;
class PlayerController;
class Transform2D;

class PopUpController : public Script
{
    DECLARE_SCRIPT(PopUpController)

public:
    explicit PopUpController(GameObject* owner);

    void Start() override;
    void Update() override;

    void startPopUp(PopUpEvent* event);

    bool isShowingPopUp() const { return m_isShowingPopUp; }

private:
    enum class PopUpState
    {
        None,
        FadingIn,
        WaitingForConfirmation,
        FadingOut
    };

private:
    void updateFadingIn(float dt);
    void updateWaitingForConfirmation();
    void updateFadingOut(float dt);

    void finishPopUp();

    void findPlayerControllers();
    void setPlayersGameplayInputLocked(bool locked);

    void setPopUpAlpha(float alpha);

private:
    PopUpEvent* m_currentEvent = nullptr;
    Transform2D* m_currentPopUpImage = nullptr;

    std::vector<PlayerController*> m_playerControllers;

    PopUpState m_state = PopUpState::None;

    bool m_isShowingPopUp = false;

    bool m_player1Confirmed = false;
    bool m_player2Confirmed = false;

    float m_timer = 0.0f;
    float m_currentAlpha = 0.0f;
};
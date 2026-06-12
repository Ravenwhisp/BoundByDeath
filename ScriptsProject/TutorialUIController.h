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

    void finishTutorialUI();

    void findPlayerControllers();
    void setPlayersGameplayInputLocked(bool locked);
    void setPlayersInvulnerable(bool invulnerable);

    void setTutorialUIAlpha(float alpha);

private:
    TutorialUIEvent* m_currentEvent = nullptr;
    Transform2D* m_currentTutorialUIImage = nullptr;

    std::vector<PlayerController*> m_playerControllers;
    std::vector<Damageable*> m_playerDamageables;

    TutorialUIState m_state = TutorialUIState::None;

    bool m_isShowingTutorialUI = false;

    bool m_player1Confirmed = false;
    bool m_player2Confirmed = false;

    float m_timer = 0.0f;
    float m_currentAlpha = 0.0f;
};
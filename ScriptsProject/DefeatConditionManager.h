#pragma once

#include "ScriptAPI.h"

class Transform;
class PlayerState;
class PlayerDownState;
class BarkManager;
class CooperativeSound;

class DefeatConditionManager : public Script
{
    DECLARE_SCRIPT(DefeatConditionManager)

public:
    explicit DefeatConditionManager(GameObject* owner);

    void Start() override;
    void Update() override;
    FieldList getExposedFields() const override;

    bool hasTriggeredDefeat() const { return m_hasTriggeredDefeat; }

private:
    PlayerState* findPlayerStateFromReference(Transform* transform) const;
    PlayerDownState* findPlayerDownStateFromReference(Transform* transform) const;

    void triggerDefeat();

public:
    ComponentRef<Transform> m_player1Transform;
    ComponentRef<Transform> m_player2Transform;

private:
    PlayerState* m_player1State = nullptr;
    PlayerState* m_player2State = nullptr;

    PlayerDownState* m_player1DownState = nullptr;
    PlayerDownState* m_player2DownState = nullptr;

    CooperativeSound* m_cooperativeSound = nullptr;

    bool m_hasTriggeredDefeat = false;

    bool m_defeatCountdownStarted = false;
    BarkManager* m_barkManager = nullptr;

    float m_defeatTimer = 0.0f;
    float m_defeatDelay = 3.0f;

    // Tras la cuenta atras se espera a que acabe la frase de caida: la pantalla de
    // derrota cortaba a media palabra el "Forgive me, little one". El tope evita que
    // una cola larga deje la partida colgada.
    float m_maxWaitForVoice = 8.0f;

};
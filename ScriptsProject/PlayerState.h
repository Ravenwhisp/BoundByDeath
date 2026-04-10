#pragma once

#include "ScriptAPI.h"

enum class PlayerLifeState
{
    Normal = 0,
    Downed
};

class PlayerState : public Script
{
    DECLARE_SCRIPT(PlayerState)

public:
    explicit PlayerState(GameObject* owner);

    PlayerLifeState getState() const { return static_cast<PlayerLifeState>(m_state); }
    void setState(PlayerLifeState state);

    bool isDowned() const { return getState() == PlayerLifeState::Downed; }
    bool canMove() const { return getState() == PlayerLifeState::Normal; }

private:
    int m_state = static_cast<int>(PlayerLifeState::Normal);
};
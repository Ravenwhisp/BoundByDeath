#include "pch.h"
#include "PlayerState.h"

PlayerState::PlayerState(GameObject* owner)
    : Script(owner)
{
}

void PlayerState::setState(PlayerStateType state)
{
    m_state = static_cast<int>(state);
}

IMPLEMENT_SCRIPT(PlayerState)
#include "pch.h"
#include "PlayerState.h"

static const char* playerStateNames[] =
{
    "Normal",
    "Downed"
};

PlayerState::PlayerState(GameObject* owner)
    : Script(owner)
{
}

void PlayerState::setState(PlayerLifeState state)
{
    m_state = static_cast<int>(state);
}

IMPLEMENT_SCRIPT(PlayerState)
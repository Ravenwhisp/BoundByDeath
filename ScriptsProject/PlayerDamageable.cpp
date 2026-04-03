#include "pch.h"
#include "PlayerDamageable.h"
#include "PlayerDownState.h"


PlayerDamageable::PlayerDamageable(GameObject* owner)
    : Damageable(owner)
{
}

void PlayerDamageable::onHpDepleted()
{
    Script* script = GameObjectAPI::getScript(m_owner, "PlayerDownState");
    PlayerDownState* downState = dynamic_cast<PlayerDownState*>(script);

    if (downState)
    {
        downState->enterDownState();
        return;
    }

    Debug::warn("%s has PlayerDamageable but no PlayerDownState. Falling back to kill.", GameObjectAPI::getName(m_owner));
    Damageable::onHpDepleted();
}

IMPLEMENT_SCRIPT(PlayerDamageable)
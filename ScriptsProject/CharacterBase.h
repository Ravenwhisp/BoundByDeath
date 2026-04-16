#pragma once

#include "Damageable.h"

class PlayerTargetController;

// Abstract base class for all playable characters.
// Never instantiated directly — DeathCharacter and LyrielCharacter inherit from this.
// NO DECLARE_SCRIPT / IMPLEMENT_SCRIPT macros.
//
// Extends Damageable with:
//   - Ability mutex (canAct): only one ability may run at a time.
//   - Player index: which gamepad/keyboard slot controls this character.
//   - Sibling script references: PlayerTargetController (targeting),
//     PlayerController (movement — stored as Script* because its header is engine-only).
//
// On death:  canAct = false, PlayerController disabled (no movement while dead).
// On revive: canAct = true,  PlayerController re-enabled.
class CharacterBase : public Damageable
{
public:
    explicit CharacterBase(GameObject* owner);

    void Start() override;

    bool canAct()          const { return m_canAct; }
    void setCanAct(bool v)       { m_canAct = v; }
    int  getPlayerIndex()  const { return m_playerIndex; }

    PlayerTargetController* getTargetController() const { return m_targetController; }

protected:
    // Base HP event reactions — subclasses override and call CharacterBase::onXxx() first.
    void onDamaged(float amount) override;
    void onDeath()               override;
    void onRevive()              override;

public:
    // Which player (0 or 1) controls this character.
    // Exposed to the inspector by each concrete subclass inside getExposedFields().
    int m_playerIndex = 0;

private:
    // True when no ability is currently executing.
    // Set to false by onActivate() on any ability, back to true by onDeactivate().
    bool m_canAct = true;

    // Obtained in Start() via GameObjectAPI::getScript — never set from the inspector.
    PlayerTargetController* m_targetController = nullptr;

    // PlayerController is an engine-side script whose header is not available here.
    // Stored as Script* and controlled only via ComponentAPI::setActive
    // to stop / restore character movement on death and revive.
    Script* m_playerController = nullptr;
};

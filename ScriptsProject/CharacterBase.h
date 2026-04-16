#pragma once

#include "PlayerDamageable.h"

class PlayerTargetController;

class CharacterBase : public PlayerDamageable
{
public:
    explicit CharacterBase(GameObject* owner);

    void Start()  override;
    void Update() override;

    bool canAct()          const { return m_canAct; }
    void setCanAct(bool v)       { m_canAct = v; }
    int  getPlayerIndex()  const { return m_playerIndex; }

    PlayerTargetController* getTargetController() const { return m_targetController; }

protected:
    void onDeath()   override;
    void onRevive()  override;

public:
    int m_playerIndex = 0;

private:
    bool m_canAct = true;

    PlayerTargetController* m_targetController = nullptr;
    Script*                 m_playerController = nullptr;
};

#pragma once

#include "ScriptAPI.h"
#include "GameplayEventAction.h"

class GameplayEventTrigger;

class CombatAreaEvent : public GameplayEventAction
{
    DECLARE_SCRIPT(CombatAreaEvent)

public:
    explicit CombatAreaEvent(GameObject* owner);

    void Update() override;

    void executeEvent(GameplayEventTrigger* trigger) override;

    ScriptFieldList getExposedFields() const override;

private:
    void closeArea();
    void openArea();

    void setBlockerState(const ScriptComponentRef<Transform>& blockerTransformRef, bool blocked);

    bool areAllEnemiesDead() const;
    bool isEnemyDead(const ScriptComponentRef<Transform>& enemyTransformRef) const;

public:
    std::vector<ScriptComponentRef<Transform>> m_enemies;

    ScriptComponentRef<Transform> m_entranceBlocker;
    ScriptComponentRef<Transform> m_exitBlocker;

private:
    bool m_isActive = false;
    bool m_hasCompleted = false;
};
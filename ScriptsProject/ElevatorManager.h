#pragma once

#include "ScriptAPI.h"

class CombatAreaEvent;

class ElevatorManager : public Script
{
    DECLARE_SCRIPT(ElevatorManager)

public:
    explicit ElevatorManager(GameObject* owner);

    void Start() override;
    void Update() override;

    FieldList getExposedFields() const override;

private:
    void resolveCombatAreas();
    void enableArea(int waveIndex);
    void disableArea(int waveIndex);
    void startElevatorMove(int targetIndex);
    void updateElevatorMove();

public:
    std::vector<ComponentRef<Transform>> m_combatAreaRoots;
    ComponentRef<Transform> m_elevator;
    std::vector<ComponentRef<Transform>> m_elevatorTargets;
    float m_moveDuration = 2.0f;

private:
    std::vector<CombatAreaEvent*> m_combatAreas;
    int m_wavesStarted = 0;
    int m_wavesCompleted = 0;
    bool m_elevatorMoving = false;
    float m_elevatorTimer = 0.0f;
    Vector3 m_elevatorStartPos;
    Vector3 m_elevatorTargetPos;
    int m_elevatorTargetIndex = 0;
};

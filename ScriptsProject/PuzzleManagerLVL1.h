#pragma once

#include "ScriptAPI.h"

class CrystalShadowMark;

class PuzzleManagerLVL1 : public Script
{
    DECLARE_SCRIPT(MyScript)

public:
    explicit PuzzleManagerLVL1(GameObject* owner);

    void Start() override;
    void Update() override;

    FieldList getExposedFields() const override;

private:
    void puzzle1Solved();
    void puzzle2Solved();
    void puzzle3Solved();

    void onPuzzleSolved(int puzzleId);

    bool isPuzzleSolved(int puzzleId) const;

    void updateBridgeLowering(float dt);

public:
    ComponentRef<Transform> m_door1;
    ComponentRef<Transform> m_bridge1;
    ComponentRef<Transform> m_bridge2;
    ComponentRef<Transform> m_door2;
    ComponentRef<Transform> m_door3;
    ComponentRef<Transform> m_navBlocker1;
    ComponentRef<Transform> m_navBlocker2;
    ComponentRef<Transform> m_navBlocker3;
    ComponentRef<Transform> m_navBlocker4;

    std::vector<ComponentRef<Transform>> m_puzzle0Crystals;
    std::vector<ComponentRef<Transform>> m_puzzle1Crystals;
    std::vector<ComponentRef<Transform>> m_puzzle2Crystals;

    GameObject* blocker1;
    GameObject* blocker2;
    GameObject* blocker3;
    GameObject* blocker4;

    float m_bridgeLowerDuration = 2.0f;

private:
    struct CrystalGroup
    {
        std::vector<ComponentRef<Transform>>* refs;
        std::vector<CrystalShadowMark*> resolved;
        bool solved = false;
    };

    CrystalGroup m_groups[3];

    // Bridge lowering animation
    bool m_bridgeLowering = false;
    float m_bridgeLowerTimer = 0.0f;
    Vector3 m_bridge1StartPos, m_bridge1StartRot, m_bridge1TargetPos, m_bridge1TargetRot;
    Vector3 m_bridge2StartPos, m_bridge2StartRot, m_bridge2TargetPos, m_bridge2TargetRot;
    GameObject* m_bridgeSoundEmitter = nullptr;
};

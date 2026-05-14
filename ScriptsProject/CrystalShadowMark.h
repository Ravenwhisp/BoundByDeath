#pragma once
#include "ScriptAPI.h"
#include "EnemyShadowMark.h"
#include "PuzzleManagerLVL1.h"

class CrystalShadowMark : public Script
{
    DECLARE_SCRIPT(CrystalShadowMark)
public:
    explicit CrystalShadowMark(GameObject* owner);

    void Start()  override;
    void Update() override;

    

    void onMarkExploided();

    bool isActivated() const { return m_activated; }

    ScriptComponentRef<Transform> m_puzzleManager;

	int m_puzzleID = 0;

	float m_activeTime = 5.0f;

private:

    EnemyShadowMark* m_shadowMark = nullptr;
    bool m_activated = false;

	float m_activationTimer = 0.0f;

ScriptFieldList getExposedFields() const override;
};


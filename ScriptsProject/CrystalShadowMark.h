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

private:

    EnemyShadowMark* m_shadowMark = nullptr;
    bool m_activated = false;

ScriptFieldList getExposedFields() const override;
};


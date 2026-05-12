#pragma once

#include "ScriptAPI.h"

class PuzzleManagerLVL1 : public Script
{
    DECLARE_SCRIPT(MyScript)

public:
    explicit PuzzleManagerLVL1(GameObject* owner);

    void Start() override;
    void Update() override;

	ScriptComponentRef<Transform> m_crystal1;
	ScriptComponentRef<Transform> m_crystal2;

	ScriptComponentRef<Transform> m_door1;

	void puzzle1Solved();

private:

	bool isPuzzle1Solved() const;

    ScriptFieldList getExposedFields() const override;
};


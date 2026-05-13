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

	void onCrystalsActivated();

private:

	int m_crystalsActivated = 0;

	static constexpr int k_totalCrystals = 2;

	bool isPuzzle1Solved() const;

    ScriptFieldList getExposedFields() const override;
};


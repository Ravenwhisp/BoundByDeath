#include "pch.h"
#include "PuzzleManagerLVL1.h"

IMPLEMENT_SCRIPT_FIELDS(PuzzleManagerLVL1,
    SERIALIZED_COMPONENT_REF(m_crystal1, "Cristal1", ComponentType::TRANSFORM),
	SERIALIZED_COMPONENT_REF(m_crystal2, "Cristal2", ComponentType::TRANSFORM),
	SERIALIZED_COMPONENT_REF(m_door1, "Door1", ComponentType::TRANSFORM)
)

PuzzleManagerLVL1::PuzzleManagerLVL1(GameObject* owner)
    : Script(owner)
{
}

void PuzzleManagerLVL1::Start()
{
}

void PuzzleManagerLVL1::Update()
{

}

void PuzzleManagerLVL1::puzzle1Solved()
{
	Debug::log("Puzzle 1 solved! Opening door...");
	TransformAPI::setRotationEuler(m_door1.getReferencedComponent(), Vector3(0.0f, 90.0f, 0.0f));
}

void PuzzleManagerLVL1::onCrystalsActivated()
{
	m_crystalsActivated++;
	Debug::log("Crystal activated! Total activated: %d/%d", m_crystalsActivated, k_totalCrystals);
	if (isPuzzle1Solved())
	{
		puzzle1Solved();
	}
}

bool PuzzleManagerLVL1::isPuzzle1Solved() const
{
	return m_crystalsActivated >= k_totalCrystals;
}

IMPLEMENT_SCRIPT(PuzzleManagerLVL1)
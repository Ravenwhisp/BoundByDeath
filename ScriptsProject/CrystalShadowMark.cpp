#include "pch.h"
#include "CrystalShadowMark.h"

IMPLEMENT_SCRIPT_FIELDS(CrystalShadowMark,
    SERIALIZED_COMPONENT_REF(m_puzzleManager, "PuzzleManager", ComponentType::TRANSFORM),
	SERIALIZED_INT(m_puzzleID, "Puzzle ID"),
	SERIALIZED_FLOAT(m_activeTime, "Active Time", 0.0f, 10.0f, 0.1f)
)

CrystalShadowMark::CrystalShadowMark(GameObject* owner) : Script(owner) {}

void CrystalShadowMark::Start()
{
    m_shadowMark = GameObjectAPI::findScript<EnemyShadowMark>(getOwner());
    if (m_shadowMark == nullptr)
        Debug::log("[CrystalMark] WARNING: No EnemyShadowMark found on this crystal!");
}

void CrystalShadowMark::Update() 
{
    if (!m_activated) return;

	m_activationTimer += Time::getDeltaTime();
    if (m_activationTimer >= m_activeTime)
    {
        Debug::log("[CrystalMark] Crystal deactivated after %.1f seconds.", m_activeTime);
        m_activated = false;
        m_activationTimer = 0.0f;

        GameObject* managerObject = ComponentAPI::getOwner(m_puzzleManager.getReferencedComponent());
        PuzzleManagerLVL1* manager = GameObjectAPI::findScript<PuzzleManagerLVL1>(managerObject);
        if (manager != nullptr)
        {
            manager->onCrystalsDeactivated(m_puzzleID);
        }
        else
        {
            Debug::log("[CrystalMark] WARNING: PuzzleManagerLVL1 not found!");
        }
    }
}

void CrystalShadowMark::onMarkExploided()
{
    if (m_activated) return;

    m_activated = true;
    Debug::log("[CrystalMark] Crystal activated!");

	GameObject* managerObject = ComponentAPI::getOwner(m_puzzleManager.getReferencedComponent());
    PuzzleManagerLVL1* manager = GameObjectAPI::findScript<PuzzleManagerLVL1>(managerObject);
    if (manager != nullptr)
    {
        manager->onCrystalsActivated(m_puzzleID);
    }  
    else
    {
        Debug::log("[CrystalMark] WARNING: PuzzleManagerLVL1 not found!");
    }    
}

IMPLEMENT_SCRIPT(CrystalShadowMark)

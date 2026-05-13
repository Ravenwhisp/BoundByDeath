#include "pch.h"
#include "CrystalShadowMark.h"

IMPLEMENT_SCRIPT_FIELDS(CrystalShadowMark,
    SERIALIZED_COMPONENT_REF(m_puzzleManager, "PuzzleManager", ComponentType::TRANSFORM)
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
        manager->onCrystalsActivated();
    }  
    else
    {
        Debug::log("[CrystalMark] WARNING: PuzzleManagerLVL1 not found!");
    }    
}

IMPLEMENT_SCRIPT(CrystalShadowMark)

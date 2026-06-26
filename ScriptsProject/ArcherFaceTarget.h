#pragma once
#include "ScriptAPI.h"

class AnimationComponent;
class RangedEnemyController;

class ArcherFaceTarget : public Script
{
    DECLARE_SCRIPT(ArcherFaceTarget)
public:
    explicit ArcherFaceTarget(GameObject* owner);
    void Start()  override;
    void Update() override;

private:
    AnimationComponent*    m_animation  = nullptr;
    RangedEnemyController* m_controller = nullptr;
};

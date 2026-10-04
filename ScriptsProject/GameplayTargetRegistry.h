#pragma once

#include "ScriptAPI.h"
#include <vector>

class Damageable;
class EnemyBaseController;
class CrystalShadowMark;
class BreakableObject;

// Shared list of live gameplay targets. Damageable owns registration; players only read it.
namespace GameplayTargetRegistry
{
    struct Target
    {
        GameObject* gameObject = nullptr;
        Transform* transform = nullptr;
        Damageable* damageable = nullptr;
        EnemyBaseController* enemyController = nullptr;
        CrystalShadowMark* crystalShadowMark = nullptr;
        BreakableObject* breakableObject = nullptr;
        Tag tag = Tag::DEFAULT;
    };

    void registerTarget(Damageable* damageable);
    void unregisterTarget(const Damageable* damageable);
    const std::vector<Target>& getTargets();
    bool isValid(const Target& target, bool& removedFromScene);
    void pruneRemovedTargets();
}

#include "pch.h"
#include "GameplayTargetRegistry.h"

#include "Damageable.h"
#include "EnemyBaseController.h"
#include "CrystalShadowMark.h"
#include "BreakableObject.h"
#include <algorithm>

namespace
{
    std::vector<GameplayTargetRegistry::Target>& targets()
    {
        static std::vector<GameplayTargetRegistry::Target> registeredTargets;
        return registeredTargets;
    }
}

void GameplayTargetRegistry::registerTarget(Damageable* damageable)
{
    if (damageable == nullptr)
    {
        return;
    }

    GameObject* object = damageable->getOwner();
    if (object == nullptr)
    {
        return;
    }

    const Tag tag = GameObjectAPI::getTag(object);
    if (tag != Tag::ENEMY && tag != Tag::BREAKABLE)
    {
        return;
    }

    auto& registeredTargets = targets();
    for (const Target& target : registeredTargets)
    {
        if (target.damageable == damageable)
        {
            return;
        }
    }

    Transform* transform = GameObjectAPI::getTransform(object);
    if (transform == nullptr)
    {
        return;
    }

    Target target;
    target.gameObject = object;
    target.transform = transform;
    target.damageable = damageable;
    target.tag = tag;
    if (tag == Tag::ENEMY)
    {
        target.enemyController = GameObjectAPI::findScript<EnemyBaseController>(object);
        target.crystalShadowMark = GameObjectAPI::findScript<CrystalShadowMark>(object);
    }
    else
    {
        target.breakableObject = GameObjectAPI::findScript<BreakableObject>(object);
    }

    registeredTargets.push_back(target);
}

void GameplayTargetRegistry::unregisterTarget(const Damageable* damageable)
{
    auto& registeredTargets = targets();
    registeredTargets.erase(
        std::remove_if(registeredTargets.begin(), registeredTargets.end(),
            [damageable](const Target& target) { return target.damageable == damageable; }),
        registeredTargets.end());
}

const std::vector<GameplayTargetRegistry::Target>& GameplayTargetRegistry::getTargets()
{
    return targets();
}

bool GameplayTargetRegistry::isValid(const Target& target, bool& removedFromScene)
{
    // Object removal is deferred. Membership must be checked before any cached pointer is used.
    if (target.gameObject == nullptr || !SceneAPI::containsGameObject(target.gameObject))
    {
        removedFromScene = true;
        return false;
    }

    if (!GameObjectAPI::isActiveInHierarchy(target.gameObject) ||
        target.transform == nullptr || target.damageable == nullptr ||
        target.damageable->isDead() || target.damageable->getCurrentHp() <= 0.0f)
    {
        return false;
    }

    return target.crystalShadowMark == nullptr || !target.crystalShadowMark->isPuzzleCompleted();
}

void GameplayTargetRegistry::pruneRemovedTargets()
{
    auto& registeredTargets = targets();
    registeredTargets.erase(
        std::remove_if(registeredTargets.begin(), registeredTargets.end(),
            [](const Target& target)
            {
                return target.gameObject == nullptr || !SceneAPI::containsGameObject(target.gameObject);
            }),
        registeredTargets.end());
}

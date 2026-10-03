#pragma once

#include "ScriptAPI.h"
#include <vector>

class PlayerController;
class CharacterBase;
class DeathSound;
class LyrielSound;
class EnemyBaseController;
class Damageable;
class CrystalShadowMark;
class BreakableObject;

class PlayerTargetController : public Script
{
    DECLARE_SCRIPT(PlayerTargetController)

public:
    explicit PlayerTargetController(GameObject* owner);

    void Start() override;
    void Update() override;
    void drawGizmo() override;

    FieldList getExposedFields() const override;

    GameObject* getCurrentTarget() const { return m_currentTarget; }

    GameObject* findNearbyTargetInRange(float range) const;

private:
    struct TargetCandidate
    {
        GameObject* gameObject = nullptr;
        EnemyBaseController* enemyController = nullptr;
        float distanceSq = 0.0f;
    };

    struct CachedTarget
    {
        GameObject* gameObject = nullptr;
        Transform* transform = nullptr;
        Damageable* damageable = nullptr;
        EnemyBaseController* enemyController = nullptr;
        CrystalShadowMark* crystalShadowMark = nullptr;
        BreakableObject* breakableObject = nullptr;
    };

    void updateTargetsInRange();
    void refreshNearbyCache(const Vector3& ownerPosition);
    void refreshCrystalCache();
    bool isCachedTargetValid(const CachedTarget& target) const;
    void clearInvalidCurrentTarget();
    void setDefaultEnemyTargetIfNeeded();

    GameObject* findDefaultEnemyTarget() const;

    bool canUpdateTarget() const;
    void updateCurrentTarget();
    void setCurrentTarget(GameObject* newTarget);

    Vector3 computeAimDirection() const;
    bool isAimStickValid(const Vector3& direction) const;

    bool tryComputeTargetScore(GameObject* target, const Vector3& aimDirection, float& outScore) const;
    GameObject* findBestTarget(const Vector3& aimDirection, float& outBestScore) const;
    bool shouldSwitchTarget(GameObject* candidate, const Vector3& aimDirection, float candidateScore) const;

    bool isTargetInRange(GameObject* target) const;
    bool isTargetAlive(GameObject* target) const;
    bool isTargetable(GameObject* target) const;

    bool canTargetBreakableDuringCombat(GameObject* target) const;

public:
    float m_targetRange = 8.0f;

    float m_targetConeAngle = 50.0f;
    float m_angleWeight = 0.85f;
    float m_distanceWeight = 0.15f;

    float m_switchMargin = 0.15f;
    float m_switchCooldown = 0.25f;

private:
    PlayerController* m_playerController = nullptr;
    CharacterBase* m_character = nullptr;

    GameObject* m_currentTarget = nullptr;
    GameObject* m_defaultEnemyTarget = nullptr;
    std::vector<TargetCandidate> m_targetsInRange;
    std::vector<CachedTarget> m_cachedEnemies;
    std::vector<CachedTarget> m_cachedBreakables;
    std::vector<CachedTarget> m_cachedCrystals;

    Vector3 m_enemyCacheCenter = Vector3::Zero;
    float m_enemyCacheTimer = 0.0f;
    bool m_nearbyCacheValid = false;
    bool m_crystalCacheValid = false;

    DeathSound*  m_deathSound  = nullptr;
    LyrielSound* m_lyrielSound = nullptr;

    float m_switchCooldownTimer = 0.0f;
};

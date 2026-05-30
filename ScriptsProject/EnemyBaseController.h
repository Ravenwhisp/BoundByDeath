#pragma once

#include "ScriptAPI.h"

class AnimationComponent;
class Transform;

class EnemyBaseController : public Script
{
public:
    explicit EnemyBaseController(GameObject* owner);
    virtual ~EnemyBaseController() = default;

public:
    // Target helpers
    virtual void updateCurrentTarget();
    Transform* getCurrentTarget() const { return m_currentTarget; }

    virtual bool hasValidTarget() const;

    float getDistanceToCurrentTarget() const;
    bool isCurrentTargetInRange(float range) const;

    // Facing helpers
    void faceCurrentTarget();
    void facePosition(const Vector3& worldPosition);

    // Movement/path helpers
    virtual void clearPath();
    virtual void resetRepathTimer();

    // Death helpers
    bool isDead() const;
    bool trySendDeathTrigger(AnimationComponent* animation);

private:
    void rotateTowardsDirection(const Vector3& direction);

protected:
    virtual Transform* acquireCurrentTarget() = 0;
    virtual float getTurnSpeedDegrees() const = 0;

protected:
    Transform* m_currentTarget = nullptr;
    bool m_deathTriggerSent = false;

    std::vector<Vector3> m_path;
    size_t m_currentPathIndex = 0;
    bool m_hasPath = false;
    float m_repathTimer = 0.0f;
};
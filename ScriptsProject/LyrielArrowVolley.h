#pragma once

#include "LyrielAbilityBase.h"
#include <vector>

class Damageable;
class LyrielUI;

class LyrielArrowVolley : public LyrielAbilityBase
{
    DECLARE_SCRIPT(LyrielArrowVolley)

public:
    explicit LyrielArrowVolley(GameObject* owner);

    void Start() override;
    void Update() override;
	void updateUI() override;
    void drawGizmo() override;

protected:
	void startAbility() override;

    void onAttackWindowUpdate() override;
    void onAttackWindowFinished() override;
    void onHitFrame() override;
    void cancelAbility() override;

    float getCooldown() const override;

private:
    void beginAim();
    void updateAim();
    void releaseAimAndCast();

    bool canCast() const;

    Vector3 computeAimDirection() const;
    bool isAimStickValid(const Vector3& direction) const;

    void collectEnemiesInCone(const Vector3& origin, const Vector3& forward, std::vector<Damageable*>& outTargets);
    void applyVolleyDamage(const std::vector<Damageable*>& targets);
    void spawnVolleyArrows(const Vector3& origin, const Vector3& forward);

    // One Arrow_Impact one-shot per enemy the cone caught, fired when the volley
    // reaches it rather than all at once on release.
    void scheduleArrowImpactSfx(const Vector3& origin, const std::vector<Damageable*>& targets);
    void updateArrowImpactSfx(float deltaTime);

    void drawAimPreview(const Vector3& origin, const Vector3& forward) const;

private:
    LyrielUI* m_lyrielUI = nullptr;

    bool m_isAiming = false;
    Vector3 m_currentAimDirection = Vector3::Zero;
    Vector3 m_attackFacingDirection = Vector3::Zero;

    Vector3 m_pendingOrigin = Vector3::Zero;
    Vector3 m_pendingForward = Vector3::Zero;
    std::vector<Damageable*> m_targetScratch;
    std::vector<float> m_pendingImpactSfx;
};

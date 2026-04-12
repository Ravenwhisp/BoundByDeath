#pragma once

#include "AbilityBase.h"

class EnemyDetectionAggro;

// Death's taunt applies a cone-shaped provoke in front of the caster.
// Enemies affected by the cone keep the taunt for a fixed duration even if
// the caster later leaves their detection radius.
class DeathTaunt : public AbilityBase
{
    DECLARE_SCRIPT(DeathTaunt)

public:
    explicit DeathTaunt(GameObject* owner);

    void Start()  override;
    void Update() override;
    void drawGizmo() override;

protected:
    void onActivate() override;
    void onDeactivate() override;

private:
    void applyTauntToEnemiesInCone() const;
    bool isEnemyInsideTauntCone(GameObject* enemy, const Vector3& ownerPosition, const Vector3& ownerForward) const;

private:
    static constexpr float kTauntCooldownSeconds = 8.0f;
    static constexpr float kTauntDurationSeconds = 3.0f;
    static constexpr float kTauntRange = 2.5f;
    static constexpr float kTauntHalfAngleDegrees = 35.0f;
};

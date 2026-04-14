#pragma once
#include "ScriptAPI.h"
#include "AbilityBase.h"


class EnemyDetectionAggro;

class DeathTaunt : public AbilityBase
{
    DECLARE_SCRIPT(DeathTaunt)

public:
    explicit DeathTaunt(GameObject* owner);

    void Start()  override;
    void Update() override;

    ScriptFieldList getExposedFields() const override;
    void drawGizmo() override;

protected:
    void onActivate() override;
    void onDeactivate() override;

private:
    void applyTauntToEnemiesInCone() const;
    bool isEnemyInsideTauntCone(GameObject* enemy, const Vector3& ownerPosition, const Vector3& ownerForward) const;

public:
    float m_TauntCooldownSeconds = 8.0f;
    float m_TauntDurationSeconds = 3.0f;
    float m_TauntRange = 2.5f;
    float m_TauntHalfAngleDegrees = 35.0f;
};

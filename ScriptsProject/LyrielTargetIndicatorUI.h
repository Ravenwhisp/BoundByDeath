#pragma once

#include "TargetIndicatorUI.h"

class LyrielTargetIndicatorUI : public TargetIndicatorUI
{
    DECLARE_SCRIPT(LyrielTargetIndicatorUI)

public:
    explicit LyrielTargetIndicatorUI(GameObject* owner);

    ScriptFieldList getExposedFields() const override;

protected:
    void onStart() override;
    void updateDirectionIndicator(GameObject* currentTarget) override;
    void hideDirectionIndicator() override;

private:
    bool tryGetDirectionToTarget(GameObject* currentTarget, Vector3& outPlayerPosition, Vector3& outDirection) const;
    void updateDirectionArrowTransform(Transform* arrowTransform, const Vector3& position, const Vector3& direction) const;

public:
    ScriptComponentRef<Transform> m_directionArrowTransform;

    float m_floorYOffset = 0.05f;
    float m_forwardOffset = 1.0f;
    float m_rotationOffsetDegrees = 0.0f;

private:
    Vector3 m_directionArrowBaseScale = Vector3(1.0f, 1.0f, 1.0f);
};
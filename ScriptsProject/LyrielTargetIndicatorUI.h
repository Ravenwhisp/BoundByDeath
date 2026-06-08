#pragma once

#include "TargetIndicatorUI.h"

class LyrielTargetIndicatorUI : public TargetIndicatorUI
{
    DECLARE_SCRIPT(LyrielTargetIndicatorUI)

public:
    explicit LyrielTargetIndicatorUI(GameObject* owner);

    ScriptFieldList getExposedFields() const override;

protected:
    void updateDirectionIndicator(GameObject* currentTarget) override;
    void hideDirectionIndicator() override;

private:
    bool tryGetDirectionToTarget(Vector3& outOriginPosition, Vector3& outDirection) const;
    void updateDirectionArrowTransform(Transform* arrowTransform, const Vector3& originPosition, const Vector3& direction) const;

public:
    ScriptComponentRef<Transform> m_directionArrowTransform;
    ScriptComponentRef<Transform> m_directionOriginTransform;

    float m_forwardOffset = 1.0f;
    float m_rotationOffsetDegrees = 0.0f;
};
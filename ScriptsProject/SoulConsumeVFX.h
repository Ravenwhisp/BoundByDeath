#pragma once

#include "ScriptAPI.h"

class SoulConsumeVFX : public Script
{
    DECLARE_SCRIPT(SoulConsumeVFX)

public:
    explicit SoulConsumeVFX(GameObject* owner);

    void Start() override;
    void Update() override;
    void drawGizmo() override;

    FieldList getExposedFields() const override;

    // Called immediately after the prefab is spawned by EnemyShadowMark.
    void beginFlight(Transform* target);

public:
    float m_flightDuration = 0.65f;
    float m_initialVerticalLift = 1.2f;
    float m_verticalLiftRandomness = 0.35f;
    float m_lateralCurveRange = 1.0f;
    float m_terminalCurveInfluence = 0.35f;
    Vector3 m_targetOffset = Vector3(0.0f, 1.0f, 0.0f);
    float m_trailFadeDelay = 0.25f;
    bool m_drawDebugCurve = false;

private:
    Vector3 getTargetPosition() const;
    Vector3 evaluateBezier(float t) const;
    void updateFacingDirection(const Vector3& previousPosition, const Vector3& newPosition);
    void setTrailGenerating(GameObject* gameObject, bool generate) const;
    void beginTrailFade();

    Transform* m_ownerTransform = nullptr;
    Transform* m_targetTransform = nullptr;

    Vector3 m_startPosition = Vector3::Zero;
    Vector3 m_firstControlPoint = Vector3::Zero;
    Vector3 m_lateralDirection = Vector3::Zero;
    float m_lateralOffset = 0.0f;

    bool m_isFlying = false;
    bool m_isFadingTrail = false;
    float m_flightTimer = 0.0f;
    float m_trailFadeTimer = 0.0f;
    Vector3 m_previousPosition = Vector3::Zero;
};
